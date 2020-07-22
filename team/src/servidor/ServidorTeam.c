//
// Created by Alan Zhao on 07/05/2020.
//

#include "servidor/ServidorTeam.h"

void configurarServer() {

	/*	t_config* configServer = config_create(TEAM_CONFIG_FILE);

	 IP_Broker = string_new();
	 string_append(&IP_Broker, config_get_string_value(configServer, "IP_BROKER"));

	 Puerto_Broker = string_new();
	 string_append(&Puerto_Broker, config_get_string_value(configServer, "PUERTO_BROKER"));

	 IP_Team_Gameboy = string_new();
	 string_append(&IP_Team_Gameboy, config_get_string_value(configServer, "IP_TEAM_GAMEBOY"));

	 Puerto_Team_Gameboy = string_new();
	 string_append(&Puerto_Team_Gameboy, config_get_string_value(configServer, "PUERTO_TEAM_GAMEBOY"));

	 Tiempo_Reconexion = config_get_int_value(configServer, "TIEMPO_RECONEXION");

	 Id_Team = config_get_int_value(configServer, "ID_TEAM");

	 config_destroy(configServer);*/

	IP_Broker = string_new();
	string_append(&IP_Broker, servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_BROKER));

	Puerto_Broker = string_new();
	string_append(&Puerto_Broker, servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_BROKER));

	IP_Team_Gameboy = string_new();
	string_append(&IP_Team_Gameboy, servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_TEAM_GAMEBOY));

	Puerto_Team_Gameboy = string_new();
	string_append(&Puerto_Team_Gameboy, servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_TEAM_GAMEBOY));

	Tiempo_Reconexion = servicioDeConfiguracion.obtenerEntero(&servicioDeConfiguracion, TIEMPO_RECONEXION);

	Id_Team = servicioDeConfiguracion.obtenerEntero(&servicioDeConfiguracion, ID_TEAM);
}

void eliminarConfigServer() {
	//Llamar a esta función en apagarServer
	free(IP_Broker);
	free(Puerto_Broker);
	free(IP_Team_Gameboy);
	free(Puerto_Team_Gameboy);
}

void atenderConexiones() {

	pthread_create(&threadAppeared, NULL, (void*) atenderAppeared, NULL);
	pthread_detach(threadAppeared);

	pthread_create(&threadLocalized, NULL, (void*) atenderLocalized, NULL);
	pthread_detach(threadLocalized);

	pthread_create(&threadCaught, NULL, (void*) atenderCaught, NULL);
	pthread_detach(threadCaught);

	pthread_create(&threadGameboy, NULL, (void*) atenderGameboy, NULL);
	pthread_detach(threadGameboy);
}

void apagarServer() {
	close(SOCKET_APPEARED);
	close(SOCKET_CAUGHT);
	close(SOCKET_LOCALIZED);
	close(SOCKET_GAMEBOY);

	pthread_cancel(threadGameboy);
	pthread_cancel(threadAppeared);
	pthread_cancel(threadLocalized);
	pthread_cancel(threadCaught);
	log_info(INTERNAL_LOGGER, "El servidor fue apagado.");
	eliminarConfigServer();
	//con esta tecnica el leakeo, que deberia ser del tipo still reachable, va a ser solo cuando cerremos team, en memoria pedida por funciones que hacen recv.
}

void atenderAppeared() {
	SOCKET_APPEARED = subscribirseACola(APPEARED, MANDATORY_LOGGER);
	esperarBrokerAppeared(SOCKET_APPEARED);
}

void esperarBrokerAppeared(int socketDeEscucha) {
	while (1) {

		uint32_t idMensaje;
		Pokemon* unPokemon = recv_pokemon(socketDeEscucha, 0);
		int flagError = 0;

		if (unPokemon == NULL) {
			flagError = 1;
		}
		if (recv(socketDeEscucha, &idMensaje, sizeof(uint32_t), 0) <= 0) {
			flagError = 1;
		}
		Result ack = ACKNOWLEDGE;
		if (send(socketDeEscucha, &ack, sizeof(int), MSG_NOSIGNAL) < 0) {
			flagError = 1;
		}

		if (flagError) {
			log_error(MANDATORY_LOGGER, "Cola: APPEARED. Se perdió la conexión con el Broker. Iniciando reconexión.");
			close(socketDeEscucha);
			socketDeEscucha = subscribirseACola(APPEARED, MANDATORY_LOGGER);
		} else {
			ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
			argumentosHilo->mensaje = unPokemon;
			argumentosHilo->idMensaje = idMensaje;

			pthread_t thread;
			pthread_create(&thread, NULL, (void*) procesarHiloAppeared, argumentosHilo);
			pthread_detach(thread);
		}
	}
}

void procesarHiloAppeared(ArgumentosHilo* argumentosHilo) {
	Pokemon* unAppeared = (Pokemon*) argumentosHilo->mensaje;
	uint32_t idMensaje = argumentosHilo->idMensaje;

	manejadorDeEventosProcesoTeam->procesarAppearedRecibido(manejadorDeEventosProcesoTeam, unAppeared, idMensaje);
}

void atenderCaught() {
	SOCKET_CAUGHT = subscribirseACola(CAUGHT, MANDATORY_LOGGER);
	esperarBrokerCaught(SOCKET_CAUGHT);
}

void esperarBrokerCaught(int socketDeEscucha) {
	while (1) {
		uint32_t idMensaje;
		Caught* unCaught = recv_caught(socketDeEscucha);
		int flagError = 0;
		if (unCaught == NULL) {
			flagError = 1;
		}

		if (recv(socketDeEscucha, &idMensaje, sizeof(uint32_t), 0) <= 0) {
			flagError = 1;
		}

		Result ack = ACKNOWLEDGE;
		if (send(socketDeEscucha, &ack, sizeof(int), MSG_NOSIGNAL) < 0) {
			flagError = 1;
		}

		if (flagError) {
			log_error(MANDATORY_LOGGER, "Cola: CAUGHT, Se perdió la conexión con el Broker. Iniciando reconexión.");
			close(socketDeEscucha);
			socketDeEscucha = subscribirseACola(CAUGHT, MANDATORY_LOGGER);
		} else {

			ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
			argumentosHilo->mensaje = unCaught;
			argumentosHilo->idMensaje = idMensaje;

			pthread_t thread;
			pthread_create(&thread, NULL, (void*) procesarHiloCaught, argumentosHilo);
			pthread_detach(thread);
		}
	}
}

void procesarHiloCaught(ArgumentosHilo* argumentosHilo) {
	Caught* unCaught = (Caught*) argumentosHilo->mensaje;
	uint32_t idMensaje = argumentosHilo->idMensaje;
	//
	manejadorDeEventosProcesoTeam->procesarCaughtRecibido(manejadorDeEventosProcesoTeam, unCaught, idMensaje);
}

void atenderLocalized() {
	SOCKET_LOCALIZED = subscribirseACola(LOCALIZED, MANDATORY_LOGGER);
	esperarBrokerLocalized(SOCKET_LOCALIZED);
}

void esperarBrokerLocalized(int socketDeEscucha) {
	while (1) {

		uint32_t idMensaje;
		Localized* unLocalized = recv_localized(socketDeEscucha);
		int flagError = 0;

		if (unLocalized == NULL) {
			flagError = 1;

		}
		if (recv(socketDeEscucha, &idMensaje, sizeof(uint32_t), 0) <= 0) {
			flagError = 1;

		}
		Result ack = ACKNOWLEDGE;
		if (send(socketDeEscucha, &ack, sizeof(int), MSG_NOSIGNAL) < 0) {
			flagError = 1;
		}

		if (flagError) {
			log_error(MANDATORY_LOGGER, "Cola: LOCALIZED. Se perdió la conexión con el Broker. Iniciando reconexión.");
			close(socketDeEscucha);
			socketDeEscucha = subscribirseACola(LOCALIZED, MANDATORY_LOGGER);
		} else {

			ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
			argumentosHilo->mensaje = unLocalized;
			argumentosHilo->idMensaje = idMensaje;

			pthread_t thread;
			pthread_create(&thread, NULL, (void*) procesarHiloLocalized, argumentosHilo);
			pthread_detach(thread);
		}
	}
}

void procesarHiloLocalized(ArgumentosHilo* argumentosHilo) {
	Localized* unLocalized = (Localized*) argumentosHilo->mensaje;
	uint32_t idMensaje;

	manejadorDeEventosProcesoTeam->procesarAppearedRecibido(manejadorDeEventosProcesoTeam, unLocalized, idMensaje);
}

void atenderGameboy() {
	SOCKET_GAMEBOY = crearSocketServidor(IP_Team_Gameboy, Puerto_Team_Gameboy);

	pthread_mutex_lock(&MTX_INTERNAL_LOG);
	log_info(INTERNAL_LOGGER, "Esperando Gameboys en el socket: '%d'", SOCKET_GAMEBOY);
	pthread_mutex_unlock(&MTX_INTERNAL_LOG);

	while (1) {

		pthread_t thread;

		struct sockaddr_in dir_cliente;
		socklen_t tam_direccion = sizeof(struct sockaddr_in);

		int socketCliente = accept(SOCKET_GAMEBOY, (void*) &dir_cliente, &tam_direccion);

		pthread_mutex_lock(&MTX_INTERNAL_LOG);
		log_info(INTERNAL_LOGGER, "Se conectó un Gameboy en el socket: '%d'", socketCliente);
		pthread_mutex_unlock(&MTX_INTERNAL_LOG);

		int codOp;
		recv(socketCliente, &codOp, sizeof(int), MSG_WAITALL);

		pthread_mutex_lock(&MTX_INTERNAL_LOG);
		log_info(INTERNAL_LOGGER, "Se recibió un: '%s' a traves de un Gameboy. Procesando....", traducirOperacion(codOp));
		pthread_mutex_unlock(&MTX_INTERNAL_LOG);

		if (codOp == APPEARED) {
			Pokemon* unPokemon = recv_pokemon(socketCliente, 0);
			close(socketCliente);

			ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
			argumentosHilo->mensaje = unPokemon;
			argumentosHilo->idMensaje = UINT32_MAX;

			pthread_create(&thread, NULL, (void*) procesarHiloAppeared, argumentosHilo);
			pthread_detach(thread);
		} else {
			log_error(INTERNAL_LOGGER, "El gameboy envio un mensaje erroneo.");
			close(socketCliente);
		}
	}
}

int iniciarSocketDeEscucha(Operation cola) {
	int socketDeEscucha = crearSocketCliente(IP_Broker, Puerto_Broker);
	if (socketDeEscucha == -1) {
		return -1;
	}
	//conectarnos al broker y hacer handshake
	Operation subscribe = SUBSCRIBE;
	Process team = TEAM;

	if (send(socketDeEscucha, &subscribe, sizeof(int), MSG_NOSIGNAL) < 0) {
		close(socketDeEscucha);
		return -1;
	}
	if (send(socketDeEscucha, &team, sizeof(int), MSG_NOSIGNAL) < 0) {
		close(socketDeEscucha);
		return -1;
	}
	if (send(socketDeEscucha, &cola, sizeof(int), MSG_NOSIGNAL) < 0) {
		close(socketDeEscucha);
		return -1;
	}
	if (send(socketDeEscucha, &Id_Team, sizeof(int), MSG_NOSIGNAL) < 0) {
		close(socketDeEscucha);
		return -1;
	}
	Result result;

	if (recv(socketDeEscucha, &result, sizeof(Result), 0) <= 0) {
		close(socketDeEscucha);
		return -1;
	}
	if (result != OK) {
		close(socketDeEscucha);
		return -1;
	}

	return socketDeEscucha;
}

int subscribirseACola(Operation cola, t_log* logger) {
	int socketDeEscucha = iniciarSocketDeEscucha(cola);
	while (socketDeEscucha == -1) {
		log_error(MANDATORY_LOGGER, "Cola: %s. Error al conectar con Broker. Reintentando en '%d' segundos...", traducirOperacion(cola), Tiempo_Reconexion);
		sleep(Tiempo_Reconexion);
		socketDeEscucha = iniciarSocketDeEscucha(cola);
	}
	if (socketDeEscucha > 0) {
		log_info(MANDATORY_LOGGER, "Cola: %s. Subscripto al Broker con el socket: '%d'. Escuchando mensajes...", traducirOperacion(cola), socketDeEscucha);
	}
	return socketDeEscucha;
}

char* traducirOperacion(Operation operacion) {

	switch (operacion) {
	case APPEARED:
		return "APPEARED";
	case CAUGHT:
		return "CAUGHT";
	case LOCALIZED:
		return "LOCALIZED";
	default:
		return "OPERACIÓN DESCONICDA. ALGO ANDA MAL!";
	}
}

char* traducirResult(Result result) {
	switch (result) {
	case OK:
		return "OK";
	case FAIL:
		return "FAIL";
	case ACKNOWLEDGE:
		return "ACKONWLEDGE";
	default:
		return "RESULTADO DESCONOCIDO. ALGO ANDA MAL!";
	}
}

char* logCoordenadas(t_list* listaCoor) {
	char* ret = string_new();
	for (int a = 0; a < listaCoor->elements_count; a++) {
		if (a == 0)
			string_append_with_format(&ret, ", coordenadas: (%d,%d)", ((Coordinate*) (list_get(listaCoor, a)))->pos_x, ((Coordinate*) (list_get(listaCoor, a)))->pos_y);
		else
			string_append_with_format(&ret, "|(%d,%d)", ((Coordinate*) (list_get(listaCoor, a)))->pos_x, ((Coordinate*) (list_get(listaCoor, a)))->pos_y);
	}
	return ret;
}
