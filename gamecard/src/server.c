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
	pthread_t threadNew, threadCatch, threadGet, threadGameboy;

	pthread_create(&threadNew, NULL, (void*) atenderNew, NULL);
	pthread_detach(threadNew);

	pthread_create(&threadCatch, NULL, (void*) atenderCatch, NULL);
	pthread_detach(threadCatch);

	pthread_create(&threadGet, NULL, (void*) atenderGet, NULL);
	pthread_detach(threadGet);

	pthread_create(&threadGameboy, NULL, (void*) atenderGameboy, NULL);
	pthread_detach(threadGameboy);

}

void atenderNew() {
	int socketDeEscucha = iniciarSocketDeEscucha();
	esperarBrokerNew(socketDeEscucha);
}

void esperarBrokerNew(int socketDeEscucha) {
	/*
	 * Ciclo que:
	 * Recibe un New
	 * Crea un hilo para atenderlo
	 */
	while (1) {
		New* unNew = recv_new(socketDeEscucha);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloNew, unNew);
		pthread_detach(thread);
	}
}

void procesarHiloNew(New* unNew) {
	//procesar
	//crear socket descartable al broker
	int socketDescartable = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
	//enviar respuesta al broker
	//esperar confirmacion del broker?
	//liberar memoriar y cerrar socket
	//termina el hilo
}

void atenderCatch() {
	int socketDeEscucha = iniciarSocketDeEscucha();
	esperarBrokerCatch(socketDeEscucha);
}

void esperarBrokerCatch(int socketDeEscucha) {
	/*
	 * Ciclo que:
	 * Recibe un New
	 * Crea un hilo para atenderlo
	 */
	while (1) {
		Pokemon* unCatch = recv_pokemon(socketDeEscucha, false);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloCatch, unCatch);
		pthread_detach(thread);
	}
}

void procesarHiloCatch(Pokemon* unCatch) {
	//procesar
	//crear socket descartable al broker
	int socketDescartable = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
	//enviar respuesta al broker
	//esperar confirmacion del broker?
	//liberar memoriar y cerrar socket
	//termina el hilo
}

void atenderGet() {
	int socketDeEscucha = iniciarSocketDeEscucha();
	esperarBrokerGet(socketDeEscucha);
}

void esperarBrokerGet(int socketDeEscucha) {
	/*
	 * Ciclo que:
	 * Recibe un New
	 * Crea un hilo para atenderlo
	 */
	while (1) {
		Get* unGet = recv_get(socketDeEscucha);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloGet, unGet);
		pthread_detach(thread);
	}
}

void procesarHiloGet(Get* unGet) {
	//procesar
	//crear socket descartable al broker
	int socketDescartable = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
	//enviar respuesta al broker
	//esperar confirmacion del broker?
	//liberar memoriar y cerrar socket
	//termina el hilo
}

void atenderGameboy() {
	int socketServidor = crearSocketServidor(IP_GAMECARD_GAMEBOY,
			PUERTO_GAMECARD_GAMEBOY);



	return;
}

int iniciarSocketDeEscucha() {
	int socketDeEscucha = crearSocketCliente(IP_BROKER, PUERTO_BROKER);
	//conectarnos al broker y hacer handshake

	send(socketDeEscucha, (int*) 200, sizeof(int), 0);

	printf("Envie: %d", 200);

	int recibido;
	recv(socketDeEscucha, &recibido, sizeof(int), MSG_WAITALL);

	printf("Recibi: %d", recibido);

//antes del return validar que se recibio bien.
	return socketDeEscucha;

}

