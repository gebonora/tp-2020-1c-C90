//
// Created by Alan Zhao on 07/05/2020.
//

#include "servidor/ServidorTeam.h"

void configurarServer() {

	configServer = config_create(TEAM_CONFIG_FILE);

	IP_Broker = config_get_string_value(configServer, "IP_BROKER");
	Puerto_Broker = config_get_string_value(configServer, "PUERTO_BROKER");

	IP_Team_Gameboy = config_get_string_value(configServer, "IP_TEAM_GAMEBOY");
	Puerto_Team_Gameboy = config_get_string_value(configServer, "PUERTO_TEAM_GAMEBOY");

	Tiempo_Reconexion = config_get_int_value(configServer, "TIEMPO_RECONEXION");

	printf("%d\n", Tiempo_Reconexion);

}

void eliminarConfigServer() {
	free(IP_Broker);
	free(Puerto_Broker);
	free(IP_Team_Gameboy);
	free(Puerto_Team_Gameboy);
	config_destroy(configServer);
}

void atenderConexiones() {

	pthread_t threadAppeared, threadCaught, threadLocalized;

	pthread_create(&threadAppeared, NULL, (void*) atenderAppeared, NULL);
	pthread_detach(threadAppeared);

	pthread_create(&threadLocalized, NULL, (void*) atenderLocalized, NULL);
	pthread_detach(threadLocalized);

	pthread_create(&threadCaught, NULL, (void*) atenderCaught, NULL);
	pthread_detach(threadCaught);

	atenderGameboy();

}

void atenderAppeared() {
	int socketDeEscucha = subscribirseACola(APPEARED, MANDATORY_LOGGER);
	esperarBrokerAppeared(socketDeEscucha);
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
			log_error(MANDATORY_LOGGER, "Se perdió la conexión con el Broker. Iniciando reconexión.");
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
	char* aux;
	if (idMensaje == UINT32_MAX) {
		aux = string_from_format("gameboy");
	} else {
		aux = string_from_format("%d", idMensaje);
	}
	log_info(MANDATORY_LOGGER, "Llegó un Appeared. idMensaje: '%s', pokemon: '%s', posX: '%d', posY: '%d'.", aux, unAppeared->name->value,
			((Coordinate*) (unAppeared->coordinates->head->data))->pos_x, ((Coordinate*) (unAppeared->coordinates->head->data))->pos_y);
	free(aux);
	//Pasar el paquete y el id a otro subproceso. ver donde se va a liberar la memoria!
	//Si se termina el hilo, la memoria se libera????? -> crear una copia y pasar la copia.
	//cerrar hilo
}

void atenderCaught() {
	int socketDeEscucha = subscribirseACola(CAUGHT, MANDATORY_LOGGER);
	esperarBrokerCaught(socketDeEscucha);
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
			log_error(MANDATORY_LOGGER, "Se perdió la conexión con el Broker. Iniciando reconexión.");
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
	log_info(MANDATORY_LOGGER, "Llegó un Caught. idMensaje: '%d', resultado: '%s'.", idMensaje, traducirResult(unCaught->result));
	//Pasar el paquete y el id a otro subproceso. ver donde se va a liberar la memoria!
	//Si se termina el hilo, la memoria se libera????? -> crear una copia y pasar la copia.
	//cerrar hilo
}

void atenderLocalized() {
	int socketDeEscucha = subscribirseACola(LOCALIZED, MANDATORY_LOGGER);
	esperarBrokerLocalized(socketDeEscucha);
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
			log_error(MANDATORY_LOGGER, "Se perdió la conexión con el Broker. Iniciando reconexión.");
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
	char* stringCoor = logCoordenadas(unLocalized->pokemon->coordinates);
	log_info(MANDATORY_LOGGER, "Llegó un Localized. idMensaje: '%d', pokemon: '%s', cantidadCoordenadas: '%d'%s.", idMensaje, unLocalized->pokemon->name->value,
			unLocalized->coordinates_quantity, stringCoor);
	free(stringCoor);
	//Pasar el paquete y el id a otro subproceso. ver donde se va a liberar la memoria!
	//Si se termina el hilo, la memoria se libera????? -> crear una copia y pasar la copia.
	//cerrar hilo
}

void atenderGameboy() {
	int socketServidor = crearSocketServidor(IP_Team_Gameboy, Puerto_Team_Gameboy);

	pthread_mutex_lock(&MTX_INTERNAL_LOG);
	log_info(INTERNAL_LOGGER, "Esperando Gameboys en el socket: '%d'", socketServidor);
	pthread_mutex_unlock(&MTX_INTERNAL_LOG);

	while (1) {

		pthread_t thread;

		struct sockaddr_in dir_cliente;
		socklen_t tam_direccion = sizeof(struct sockaddr_in);

		int socketCliente = accept(socketServidor, (void*) &dir_cliente, &tam_direccion);

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
		log_error(logger, "Error al conectar con Broker. Reintentando en '%d' segundos...", Tiempo_Reconexion);
		sleep(Tiempo_Reconexion);
		socketDeEscucha = iniciarSocketDeEscucha(cola);
	}
	log_info(logger, "Subscripto al Broker con el socket: '%d' Escuchando mensajes...", socketDeEscucha);

	return socketDeEscucha;
}

char* traducirOperacion(Operation operacion) {
	if (operacion == APPEARED) {
		return "APPEARED";
	} else {
		return "MENSAJE ERRONEO";
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
			string_append_with_format(&ret, " ,coordenadas: '(%d,%d)'", ((Coordinate*) (list_get(listaCoor, a)))->pos_x, ((Coordinate*) (list_get(listaCoor, a)))->pos_y);
		else
			string_append_with_format(&ret, "|'(%d,%d)'", ((Coordinate*) (list_get(listaCoor, a)))->pos_x, ((Coordinate*) (list_get(listaCoor, a)))->pos_y);
	}
	return ret;
}
