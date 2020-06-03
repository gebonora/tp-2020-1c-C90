/*
 * server.h
 *
 *  Created on: 19 abr. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_SERVER_H_
#define GAMECARD_INCLUDE_SERVER_H_

#include "fileSystem.h"
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <delibird/interface.h>
#include <netdb.h>
#include <config.h>

void atenderConexiones();
void atenderNew();
void esperarBrokerNew(int socketDeEscucha);
void procesarHiloNew(New* unNew);

void atenderCatch();
void esperarBrokerCatch(int socketDeEscucha);
void procesarHiloCatch(Pokemon* unCatch);

void atenderGet();
void esperarBrokerGet(int socketDeEscucha);
void procesarHiloGet(Get* unGet);

int iniciarSocketDeEscucha() ;
int crearSocketHaciaBroker();

void atenderGameboy();

#endif /* GAMECARD_INCLUDE_SERVER_H_ */
