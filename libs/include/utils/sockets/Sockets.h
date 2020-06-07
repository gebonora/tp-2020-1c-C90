/*
 * Sockets.h
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#ifndef DELIBIRD_INCLUDE_UTILS_SOCKETS_SOCKETS_H_
#define DELIBIRD_INCLUDE_UTILS_SOCKETS_SOCKETS_H_

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int crearSocketCliente(char* ip, char* puerto);
int crearSocketServidor(char* ip, char* puerto);

#endif /* DELIBIRD_INCLUDE_UTILS_SOCKETS_SOCKETS_H_ */
