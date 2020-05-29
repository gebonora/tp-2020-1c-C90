//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/gamecard/ControladorGamecard.h"

void atenderPedidoGamecard(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    log_info(logger, "Se atendio el pedido en el controlador de GAMECARD");
    //TODO: Implementar atencion
}



int crearSocketHaciaBroker() {
	char* IP = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_GAMECARD);
	char* PUERTO = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_GAMECARD);
	puts(IP);
	puts(PUERTO);
	struct addrinfo hints;
	struct addrinfo* server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(IP_BROKER, PUERTO_BROKER, &hints, &server_info);
	int socketDeEscucha = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);
	if (connect(socketDeEscucha, server_info->ai_addr, server_info->ai_addrlen)
			== -1)
		puts("error");

	freeaddrinfo(server_info);
	return socketDeEscucha;
}

ControladorGameBoy controladorGamecard = {.proceso=GAMECARD, .atenderPedido=atenderPedidoGamecard};
