//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/broker/ControladorBroker.h"
#include<readline/readline.h>
#include<stdio.h>
#include<stdlib.h>


void atenderPedidoBroker(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    log_info(logger, "Se atendio el pedido en el controlador de BROKER");
    switch(pedidoGameBoy.tipoMensaje) {
    case NEW_POKEMON: ;

    break;

    case APPEARED_POKEMON: ;
    break;

    case CATCH_POKEMON: ;
    break;
    case CAUGHT_POKEMON: ;
    break;
    case GET_POKEMON: ;
    	Get* get_pokemon = create_get_pokemon(list_get(pedidoGameBoy.argumentos, 0));
    	send_
    break;
    case UNTYPED_MESSAGE: ;
    break;
    }
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

ControladorGameBoy controladorBroker = {.proceso=BROKER, .atenderPedido=atenderPedidoBroker};
