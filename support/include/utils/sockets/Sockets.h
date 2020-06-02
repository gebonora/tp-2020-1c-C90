/*
 * Sockets.h
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#ifndef SUPPORT_INCLUDE_UTILS_SOCKETS_SOCKETS_H_
#define SUPPORT_INCLUDE_UTILS_SOCKETS_SOCKETS_H_

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int crearSocketCliente(char* ip, char* puerto);
int crearSocketServidor(char* ip, char* puerto);

#endif /* SUPPORT_INCLUDE_UTILS_SOCKETS_SOCKETS_H_ */
