
//
// Created by Alan Zhao on 07/05/2020.
//

#include "servidor/ServidorTeam.h"

void configurarServer(){

	configServer = config_create(TEAM_CONFIG_FILE);

	IP_Broker = config_get_string_value(configServer, "IP_BROKER");
	Puerto_Broker = config_get_string_value(configServer, "PUERTO_BROKER");

	IP_Team_Gameboy = config_get_string_value(configServer,"IP_TEAM_GAMEBOY");
	Puerto_Team_Gameboy = config_get_string_value(configServer,"PUERTO_TEAM_GAMEBOY");

	Tiempo_Reconexion = config_get_int_value(configServer,"TIEMPO_RECONEXION");

	printf("%d\n",Tiempo_Reconexion);

}

void eliminarConfigServer(){
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
	int socketDeEscucha = subscribirseACola(APPEARED,INTERNAL_LOGGER, &MTX_INTERNAL_LOG);
	esperarBrokerAppeared(socketDeEscucha);
}


void esperarBrokerAppeared(int socketDeEscucha) {
	while (1) {
		Pokemon* unPokemon = recv_pokemon(socketDeEscucha,0);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloAppeared, unPokemon);
		pthread_detach(thread);
	}
}

void procesarHiloAppeared(Pokemon* unPokemon) {
	//procesar

	//enviar respuesta al cliente



	//int socketDescartable = crearSocketCliente(IP_BROKER, PUERTO_BROKER); -> debo pasarlo al cliente

	//liberar memoriar
	free_pokemon(unPokemon);
	//termina el hilo
}

void atenderCaught() {
	int socketDeEscucha = subscribirseACola(CAUGHT, INTERNAL_LOGGER, &MTX_INTERNAL_LOG);
	esperarBrokerCaught(socketDeEscucha);
}



void esperarBrokerCaught(int socketDeEscucha) {

	while (1) {

		uint32_t idMensaje;
		Caught* unCaught = recv_caught(socketDeEscucha);
		recv(socketDeEscucha, &idMensaje, sizeof(uint32_t), 0);

		ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
		argumentosHilo->mensaje = unCaught;
		argumentosHilo->idMensaje = idMensaje;

		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloCaught, argumentosHilo);
		pthread_detach(thread);
	}
}


void procesarHiloCaught(ArgumentosHilo* argumentosHilo) {
	//procesar
	Caught* unCaught = (Caught*) (argumentosHilo->mensaje);
	uint32_t idMensaje = argumentosHilo->idMensaje;
	//logearCaughtRecibido(unCaught, idMensaje);


	//enviar al cliente
	//atenderCaughtRecibido(unCaught,idMensaje);
	//int socketDescartable = crearSocketHaciaBroker(); -> debe ponerse en el cliente

	//liberar memoriar y cerrar socket
	free(unCaught);
	free(argumentosHilo);
	//termina el hilo
}

void atenderLocalized() {
	int socketDeEscucha = subscribirseACola(LOCALIZED, INTERNAL_LOGGER, &MTX_INTERNAL_LOG);
	esperarBrokerLocalized(socketDeEscucha);
}

void esperarBrokerLocalized(int socketDeEscucha) {

	while (1) {
		uint32_t idMensaje;
		Localized* unLocalized = recv_localized(socketDeEscucha);
		recv(socketDeEscucha, &idMensaje, sizeof(uint32_t), 0);

		ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
		argumentosHilo->mensaje = unLocalized;
		argumentosHilo->idMensaje = idMensaje;

		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloLocalized, argumentosHilo);
		pthread_detach(thread);
	}

}

void procesarHiloLocalized(ArgumentosHilo* argumentosHilo) {
	//procesar
	Localized* unLocalized = (Localized*) (argumentosHilo->mensaje);
	uint32_t idMensaje = argumentosHilo->idMensaje;
	//logearLocalizedRecibido(unLocalized, idMensaje);

	//enviar al cliente

	//int socketDescartable = crearSocketHaciaBroker(); -> mover al cliente

	//liberar memoriar
	free_localized(unLocalized);
	free(argumentosHilo);
	//termina el hilo
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
		log_info(INTERNAL_LOGGER, "Se recibió un: '%s' a traves de un gameboy,Procesando....", traducirOperacion(codOp));
		pthread_mutex_unlock(&MTX_INTERNAL_LOG);

		if(codOp == APPEARED) {
			Pokemon* unPokemon = recv_pokemon(socketCliente,0);
			close(socketCliente);

			pthread_create(&thread, NULL, (void*) procesarHiloAppeared, unPokemon);
			pthread_detach(thread);
		}else{
			log_info(INTERNAL_LOGGER,"El gameboy envio un mensaje erroneo");
			pthread_detach(thread);
		}
	}
}

int iniciarSocketDeEscucha(Operation cola, t_log* logger, pthread_mutex_t* mutex) {
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

int subscribirseACola(Operation cola, t_log* logger, pthread_mutex_t* mutex) {
	int socketDeEscucha = iniciarSocketDeEscucha(cola, logger, mutex);
	while (socketDeEscucha == -1) {
		pthread_mutex_lock(mutex);
		log_info(logger, "Error al conectar con Broker. Reintentando en '%d' segundos...", Tiempo_Reconexion);
		pthread_mutex_unlock(mutex);
		sleep(Tiempo_Reconexion);
		socketDeEscucha = iniciarSocketDeEscucha(cola, logger, mutex);
	}
	pthread_mutex_lock(mutex);
	log_info(logger, "Subscripto al Broker con el socket: '%d' Escuchando mensajes...", socketDeEscucha);
	pthread_mutex_unlock(mutex);

	return socketDeEscucha;
}

char* traducirOperacion(Operation operacion) {
	if(operacion == APPEARED){
		return "APPEARED";
	}else{
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

