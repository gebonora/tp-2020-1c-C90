//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/suscriptor/ControladorSuscriptor.h"
#include<readline/readline.h>
#include<stdio.h>
#include "support/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"
#include<stdlib.h>
#include<netdb.h>

void atenderPedidoSuscriptor(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    log_info(logger, "Se atendio el pedido en el controlador de SUSCRIPTOR");
	char* ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_BROKER);
	char* puerto = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_BROKER);
	int socket_broker = crear_conexion(ip, puerto);
	int id;
}

ControladorGameBoy controladorSuscriptor = {.proceso=SUSCRIPTOR, .atenderPedido=atenderPedidoSuscriptor};
