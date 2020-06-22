/*
 * server.c
 *
 *  Created on: 19 abr. 2020
 *      Author: GONZALO BONORA
 */

/* Se conecta con BROKER a las colas de mensajes.
 * Abre un socket de solo escucha para un gameboy
 *
 * Le llegan mensajes de cualquiera de los dos, procesa y trata de responder al BROKER, si no puede logea error y sigue.
 *
 * Entonces para broker necesito un server?
 * Para el gameboy sí, usar un server.
 *
 * Tengo que estar tratando de levantar la conexion si se cae -> un hilo para eso.
 * Tengo que atender en principio a un broker y un gamecard (o varios?).
 *
 */

/* Recibe NEW. Responde APPEARED.
 *
 * Recibe CATCH. Responde CAUGTH.
 *
 * Recibe GET. Responde LOCALIZED.
 */
#include "server.h"

void atenderConexiones() {
	pthread_t threadNew, threadCatch, threadGet;

	pthread_create(&threadNew, NULL, (void*) atenderNew, NULL);
	pthread_detach(threadNew);

	pthread_create(&threadCatch, NULL, (void*) atenderCatch, NULL);
	pthread_detach(threadCatch);

	pthread_create(&threadGet, NULL, (void*) atenderGet, NULL);
	pthread_detach(threadGet);

	atenderGameboy();
}

void atenderNew() {
	int socketDeEscucha = subscribirseACola(NEW, loggerNew, &m_loggerNew);
	esperarBrokerNew(socketDeEscucha);
}

void esperarBrokerNew(int socketDeEscucha) {
	/*
	 * Ciclo que:
	 * Recibe un New
	 * Crea un hilo para atenderlo
	 */

	while (1) {

		uint32_t idMensaje;
		New* unNew = recv_new(socketDeEscucha); //cheqyear socket caidp y reconectar
		int flagError = 0;
		if (unNew == NULL) {
			//reconectar.
			flagError = 1;

		}
		if (recv(socketDeEscucha, &idMensaje, sizeof(uint32_t), 0) <= 0) {
			//reconectar
			flagError = 1;

		}
		Result ack = ACKNOWLEDGE;
		if (send(socketDeEscucha, &ack, sizeof(int), MSG_NOSIGNAL) < 0) {
			//reconectar
			flagError = 1;
		}

		if (flagError) {
			log_info(loggerMain, "Se perdió la conexión con el Broker. Iniciando reconexión.");
			close(socketDeEscucha);
			socketDeEscucha = subscribirseACola(NEW, loggerNew, &m_loggerNew);
		} else {

			ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
			argumentosHilo->mensaje = unNew;
			argumentosHilo->idMensaje = idMensaje;

			pthread_t thread;
			pthread_create(&thread, NULL, (void*) procesarHiloNew, argumentosHilo);
			pthread_detach(thread);
		}
	}
}

void procesarHiloNew(ArgumentosHilo* argumentosHilo) {
	//procesar
	New* unNew = (New*) (argumentosHilo->mensaje);
	uint32_t idMensaje = argumentosHilo->idMensaje;
	int flagBrokerCaido = 0;

	logearNewRecibido(unNew, idMensaje);

	//filesystem acá
	Pokemon* pokemonAppeared = procesarNew(unNew); //FS

	logearAppearedProcesado(pokemonAppeared, idMensaje);

	//crear socket descartable al broker
	int socketDescartable = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
	if (socketDescartable == -1) {
		flagBrokerCaido = 1;
	}

	//enviar respuesta al broker
	if (send_pokemon(pokemonAppeared, APPEARED, socketDescartable) < 0) {
		flagBrokerCaido = 1;
	}
	if (send(socketDescartable, &idMensaje, sizeof(uint32_t), MSG_NOSIGNAL) < 0) {
		flagBrokerCaido = 1;
	}

	//si no pudo mandar el mensaje, logeo y sigo.
	if (flagBrokerCaido) {
		pthread_mutex_lock(&m_loggerNew);
		log_info(loggerNew, "No se pudo enviar el Appeared como respuesta al mensaje: '%d' hacia el Broker. Continuando ejecución... ", idMensaje);
		pthread_mutex_unlock(&m_loggerNew);
	} else {
		pthread_mutex_lock(&m_loggerNew);
		log_info(loggerNew, "Se envió el Appeared como respuesta al mensaje: '%d' hacia el Broker.", idMensaje); //medio al pedo logear esto en la version final
		pthread_mutex_unlock(&m_loggerNew);
	}

	//liberar memoriar y cerrar socket
	free_new(unNew);
	free(argumentosHilo);
	if (socketDescartable >= 0) { //para no hacer close en un fd = -1. Es interno de nuestro programa el setearlo a mano al socketen -1.
		close(socketDescartable);
	}
	pthread_mutex_lock(&m_loggerNew);
	log_info(loggerNew, "Se terminó con éxito un hilo."); //medio al pedo logear esto en la version final
	pthread_mutex_unlock(&m_loggerNew);
	//termina el hilo
}

void atenderCatch() {
	int socketDeEscucha = subscribirseACola(CATCH, loggerCatch, &m_loggerCatch);
	esperarBrokerCatch(socketDeEscucha);
}

void esperarBrokerCatch(int socketDeEscucha) {
	/*
	 * Ciclo que:
	 * Recibe un New
	 * Crea un hilo para atenderlo
	 */
	while (1) {
		uint32_t idMensaje;
		Pokemon* unCatch = recv_pokemon(socketDeEscucha, false);
		int flagError = 0;

		if (unCatch == NULL) {
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
			log_info(loggerMain, "Se perdió la conexión con el Broker. Iniciando reconexión.");
			close(socketDeEscucha);
			socketDeEscucha = subscribirseACola(CATCH, loggerCatch, &m_loggerCatch);
		} else {

			ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
			argumentosHilo->mensaje = unCatch;
			argumentosHilo->idMensaje = idMensaje;

			pthread_t thread;
			pthread_create(&thread, NULL, (void*) procesarHiloCatch, argumentosHilo);
			pthread_detach(thread);
		}
	}
}

void procesarHiloCatch(ArgumentosHilo* argumentosHilo) {
	//procesar
	Pokemon* unPokemon = (Pokemon*) (argumentosHilo->mensaje);
	uint32_t idMensaje = argumentosHilo->idMensaje;
	int flagBrokerCaido = 0;

	logearCatchRecibido(unPokemon, idMensaje);

	//Filesystem aca:
	Caught* pokemonCaught = procesarCatch(unPokemon);

	logearCaughtProcesado(pokemonCaught, idMensaje);

	//crear socket descartable al broker
	int socketDescartable = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
	if (socketDescartable < 0) {
		flagBrokerCaido = 1;
	}
	//enviar respuesta al broker

	if (send_caught(pokemonCaught, socketDescartable) < 0) {
		flagBrokerCaido = 1;
	}
	if (send(socketDescartable, &idMensaje, sizeof(uint32_t), MSG_NOSIGNAL) < 0) {
		flagBrokerCaido = 1;
	}
	if (flagBrokerCaido) {
		pthread_mutex_lock(&m_loggerCatch);
		log_info(loggerCatch, "No se pudo enviar el Caught como respuesta al mensaje: '%d' hacia el Broker. Continuando ejecución... ", idMensaje);
		pthread_mutex_unlock(&m_loggerCatch);
	} else {
		pthread_mutex_lock(&m_loggerCatch);
		log_info(loggerCatch, "Se envió el Caught como respuesta al mensaje: '%d' hacia el Broker.", idMensaje);
		pthread_mutex_unlock(&m_loggerCatch);

	}

	//liberar memoriar y cerrar socket
	free_pokemon(unPokemon);
	free(argumentosHilo);
	if (socketDescartable >= 0) {
		close(socketDescartable);
	}
	pthread_mutex_lock(&m_loggerCatch);
	log_info(loggerCatch, "Se terminó con éxito un hilo.");
	pthread_mutex_unlock(&m_loggerCatch);
	return;
	//termina el hilo
}

void atenderGet() {
	int socketDeEscucha = subscribirseACola(GET, loggerGet, &m_loggerGet);
	esperarBrokerGet(socketDeEscucha);
}

void esperarBrokerGet(int socketDeEscucha) {
	/*
	 * Ciclo que:
	 * Recibe un New
	 * Crea un hilo para atenderlo
	 */

	while (1) {
		uint32_t idMensaje;
		Get* unGet = recv_get(socketDeEscucha);
		int flagError = 0;

		if (unGet == NULL) {
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
			log_info(loggerMain, "Se perdió la conexión con el Broker. Iniciando reconexión.");
			close(socketDeEscucha);
			socketDeEscucha = subscribirseACola(GET, loggerGet, &m_loggerGet);
		} else {

			ArgumentosHilo* argumentosHilo = malloc(sizeof(ArgumentosHilo));
			argumentosHilo->mensaje = unGet;
			argumentosHilo->idMensaje = idMensaje;

			pthread_t thread;
			pthread_create(&thread, NULL, (void*) procesarHiloGet, argumentosHilo);
			pthread_detach(thread);
		}
	}
}

void procesarHiloGet(ArgumentosHilo* argumentosHilo) {
	//procesar
	Get* unGet = (Get*) (argumentosHilo->mensaje);
	uint32_t idMensaje = argumentosHilo->idMensaje;
	int flagBrokerCaido = 0;

	logearGetRecibido(unGet, idMensaje);

	//fs aca:
	Localized* pokemonLocalized = procesarLocalized(unGet);

	//crear socket descartable al broker
	int socketDescartable = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
	if (socketDescartable < 0) {
		flagBrokerCaido = 1;
	}

	//enviar respuesta al broker
	if (send_localized(pokemonLocalized, socketDescartable) < 0) {
		flagBrokerCaido = 1;
	}

	if (send(socketDescartable, &idMensaje, sizeof(uint32_t), MSG_NOSIGNAL) < 0) {
		flagBrokerCaido = 1;
	}

	if (flagBrokerCaido) {
		pthread_mutex_lock(&m_loggerGet);
		log_info(loggerGet, "No se pudo enviar el LOCALIZED como respuesta al mensaje: '%d' hacia el Broker. Continuando ejecución... ", idMensaje);
		pthread_mutex_unlock(&m_loggerGet);
	}

	//liberar memoriar y cerrar socket
	free_get(unGet);
	free(argumentosHilo);
	if (socketDescartable >= 0) {
		close(socketDescartable);
	}
	pthread_mutex_lock(&m_loggerGet);
	log_info(loggerGet, "Se terminó con éxito un hilo");
	pthread_mutex_unlock(&m_loggerGet);
	return;
	//termina el hilo
}

void atenderGameboy() {
	int socketServidor = crearSocketServidor(IP_GAMECARD_GAMEBOY, PUERTO_GAMECARD_GAMEBOY);
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

		log_info(loggerGameboy, "Se recibió un: '%s', despachando a su flujo correspondiente...", traducirOperacion(codOp));

		switch (codOp) {
		case NEW:
			;
			New* unNew = recv_new(socketCliente);
			recv(socketCliente, &idMensaje, sizeof(uint32_t), 0);
			if (socketCliente > 0)
				close(socketCliente);

			argumentosHilo->mensaje = unNew;
			argumentosHilo->idMensaje = idMensaje;

			pthread_create(&thread, NULL, (void*) procesarHiloNew, argumentosHilo);
			pthread_detach(thread);
			break;
		case CATCH:
			;
			Pokemon* unCatch = recv_pokemon(socketCliente, false);
			recv(socketCliente, &idMensaje, sizeof(uint32_t), 0);
			if (socketCliente > 0)
				close(socketCliente);

			argumentosHilo->mensaje = unCatch;
			argumentosHilo->idMensaje = idMensaje;

			pthread_create(&thread, NULL, (void*) procesarHiloCatch, argumentosHilo);
			pthread_detach(thread);
			break;
		case GET:
			;
			Get* unGet = recv_get(socketCliente);
			recv(socketCliente, &idMensaje, sizeof(uint32_t), 0);
			if (socketCliente > 0)
				close(socketCliente);

			argumentosHilo->mensaje = unGet;
			argumentosHilo->idMensaje = idMensaje;

			pthread_create(&thread, NULL, (void*) procesarHiloGet, argumentosHilo);
			pthread_detach(thread);
			break;
		}
	}
}

int iniciarSocketDeEscucha(Operation cola, t_log* logger, pthread_mutex_t* mutex) {
	int socketDeEscucha = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
	if (socketDeEscucha == -1) {
		return -1;
	}
	//conectarnos al broker y hacer handshake
	Operation subscribe = SUBSCRIBE;
	Process gameCard = GAMECARD;

	if (send(socketDeEscucha, &subscribe, sizeof(int), MSG_NOSIGNAL) < 0) {
		close(socketDeEscucha);
		return -1;
	}
	if (send(socketDeEscucha, &gameCard, sizeof(int), MSG_NOSIGNAL) < 0) {
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
	switch (operacion) {
	case NEW:
		return "NEW";
	case CATCH:
		return "CATCH";
	case GET:
		return "GET";
	default:
		return "OPERACION DESCONOCIDA. ALGO ANDA MAL!";
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

