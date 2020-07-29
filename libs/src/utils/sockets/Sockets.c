/*
 * Sockets.c
 *
 *  Created on: 30 may. 2020
 *      Author: utnso
 */

#include "utils/sockets/Sockets.h"

int crearSocketCliente(char* ip, char* puerto) {
	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socketCliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if (connect(socketCliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		socketCliente = -1;
		perror("Error al crear Socket Cliente");
	}

	freeaddrinfo(server_info);
	return socketCliente;
}

int crearSocketServidor(char* ip, char* puerto) {
	int socketServidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socketServidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;

		if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &(int ) { 1 }, sizeof(int)) == -1)
			continue;

		if (bind(socketServidor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socketServidor);
			continue;
		}
		break;
	}
	listen(socketServidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	return socketServidor;

}
