/*
 * server.h
 *
 *  Created on: 19 abr. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_SERVER_H_
#define GAMECARD_INCLUDE_SERVER_H_


#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/sem.h>

#include <delibird/interface.h>
#include <delibird/utils/sockets/Sockets.h>

#include "fileSystem.h"
#include "config.h"

typedef struct {
	void* mensaje;
	uint32_t idMensaje;
} ArgumentosHilo;

void atenderConexiones();
void atenderNew();
void esperarBrokerNew(int socketDeEscucha);
void procesarHiloNew(ArgumentosHilo* argumentosHilo);

void atenderCatch();
void esperarBrokerCatch(int socketDeEscucha);
void procesarHiloCatch(ArgumentosHilo* argumentosHilo);

void atenderGet();
void esperarBrokerGet(int socketDeEscucha);
void procesarHiloGet(ArgumentosHilo* argumentosHilo);

int iniciarSocketDeEscucha(Operation cola, t_log* logger, pthread_mutex_t* mutex);
int subscribirseACola(Operation cola, t_log* logger, pthread_mutex_t* mutex);

void atenderGameboy();

char* traducirOperacion(Operation operacion);
char* traducirResult(Result result);

#endif /* GAMECARD_INCLUDE_SERVER_H_ */
