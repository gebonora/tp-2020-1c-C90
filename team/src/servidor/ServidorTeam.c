
//
// Created by Alan Zhao on 07/05/2020.
//

#include "servidor/ServidorTeam.h"



void atenderConexiones() {
    // TODO: convertir el request con delibird/libs.
    //  A futuro se va a utilizar un controlador para routear el pedido, como ocurre en GameBoy.

	pthread_t threadAppeared, threadCaught, threadLocalized, threadGameboy;

	pthread_create(&threadAppeared, NULL, (void*) atenderAppeared, NULL);
	pthread_detach(threadAppeared);

	pthread_create(&threadLocalized, NULL, (void*) atenderLocalized, NULL);
	pthread_detach(threadLocalized);

	pthread_create(&threadCaught, NULL, (void*) atenderCaught, NULL);
	pthread_detach(threadCaught);

	pthread_create(&threadGameboy, NULL, (void*) atenderGameboy, NULL);
	pthread_detach(threadGameboy);

	while(1);
}


void atenderAppeared() {
	int socketDeEscucha = subscribirseACola(APPEARED, loggerAppeared, &mtx_loggerAppeared);
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

	//			clienteAtiendeAppeared(unPokemon);

	//enviar respuesta al cliente

	//int socketDescartable = crearSocketCliente(IP_BROKER, PUERTO_BROKER); -> debo pasarlo al cliente
	//esperar confirmacion?
	//liberar memoriar y cerrar socket
	//termina el hilo
}

void atenderCaught() {
	int socketDeEscucha = subscribirseACola(CAUGHT, loggerCaught, &mtx_loggerCaught);
	esperarBrokerCaught(socketDeEscucha);
}



void esperarBrokerCaught(int socketDeEscucha) {

	while (1) {

		uint32_t idMensaje;
		Caught* unCaught = recv_Caught(socketDeEscucha);
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
	logearCaughtRecibido(unCaught, idMensaje);


	//			clienteAtiendeCaught(unCaught,idMensaje);
	//int socketDescartable = crearSocketHaciaBroker(); -> debe ponerse en el cliente
	//... enviar al cliente
	//liberar memoriar y cerrar socket
	//termina el hilo
	free_Caught(unCaught);
	free(argumentosHilo);
}

void atenderLocalized() {
	int socketDeEscucha = subscribirseACola(LOCALIZED, loggerLocalized, &mtx_loggerLocalized);
	esperarBrokerLocalized(socketDeEscucha);
}

void esperarBrokerLocalized(int socketDeEscucha) {

	while (1) {
		uint32_t idMensaje;
		Localized* unLocalized = recv_Localized(socketDeEscucha);
		recv(socketDeEscucha, &idMensaje, sizeof(uint32_t), 0);

		ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
		argumentosHilo->mensaje = unLocalized;
		argumentosHilo->idMensaje = idMensaje;

		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloLocalized, argumentosHilo);
		pthread_detach(thread);
	}

}

void procesarHiloLocalized(Localized* unLocalized) {
	//procesar
	Localized* unLocalized = (Localized*) (argumentosHilo->mensaje);
	uint32_t idMensaje = argumentosHilo->idMensaje;
	logearLocalizedRecibido(unLocalized, idMensaje);

	//			clienteAtiendeLocalized(unLocalized,idMensaje);
	//crear socket descartable al broker
	//int socketDescartable = crearSocketHaciaBroker(); -> mover al cliente
	//enviar respuesta al broker
	//esperar confirmacion del broker?
	//liberar memoriar y cerrar socket
	//termina el hilo
	free_Localized(unLocalized);
	free(argumentosHilo);
}




void atenderGameboy() {
	int socketServidor = crearSocketServidor(IP_TEAM_GAMEBOY, PUERTO_TEAM_GAMEBOY);
	log_info(loggerGameboy, "Esperando Gameboys en el socket: '%d'", socketServidor);
	while (1) {

		pthread_t thread;
		uint32_t idMensaje;
		ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));

		struct sockaddr_in dir_cliente;
		socklen_t tam_direccion = sizeof(struct sockaddr_in);

		int socketCliente = accept(socketServidor, (void*) &dir_cliente, &tam_direccion);

		log_info(loggerGameboy, "Se conectó un Gameboy en el socket: '%d'", socketCliente);

		int codOp;
		recv(socketCliente, &codOp, sizeof(int), MSG_WAITALL);

		log_info(loggerGameboy, "Se recibió un: '%s',Procesando....", traducirOperacion(codOp));

		if(codOp == APPEARED) {
			Pokemon* unPokemon = recv_unPokemon(socketCliente,0);
			close(socketCliente);

			pthread_create(&thread, NULL, (void*) procesarHiloAppeared, unPokemon);
			pthread_detach(thread);
		}else{
			log_error(loggerGameboy,"El gameboy envio un mensaje erroneo");
			pthread_detach(thread);
		}
}

int iniciarSocketDeEscucha(Operation cola, t_log* logger, pthread_mutex_t* mutex) {
	int socketDeEscucha = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
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
		log_info(logger, "Error al conectar con Broker. Reintentando en '%d' segundos...", TIEMPO_DE_REINTENTO_CONEXION);
		pthread_mutex_unlock(mutex);
		sleep(TIEMPO_DE_REINTENTO_CONEXION);
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

/////////////////////////////////////////////////////////////////////////////////////////////
// falta revisar logger, loguearXrecibido  para adaptarlo a team
// REVISAR RESULT

