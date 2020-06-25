//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/team/ControladorTeam.h"

void atenderPedidoTeam(PedidoGameBoy pedidoGameBoy, t_log * logger) {
	log_info(logger, "Se atendio el pedido en el controlador de TEAM");

	char* ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_TEAM);
	char* puerto = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_TEAM);
	int socketCliente;

	switch (pedidoGameBoy.tipoMensaje) {
	case APPEARED_POKEMON:
		;
		if (list_size(pedidoGameBoy.argumentos) != 3) {
			log_error(MANDATORY_LOGGER, "Los parámetros ingresados no son compatibles con APPEARED_POKEMON para TEAM");
			break;
		}

		socketCliente = crearSocketCliente(ip, puerto);

		if (socketCliente < 0) {
			log_error(MANDATORY_LOGGER, "No se pudo enviar el mensaje.");
			break;
		}
		//[POKEMON] [POSX] [POSY]
		Pokemon* appeared_pokemon = create_pokemon(list_get(pedidoGameBoy.argumentos, 0), atoi(list_get(pedidoGameBoy.argumentos, 1)),
				atoi(list_get(pedidoGameBoy.argumentos, 2)));
		send_pokemon(appeared_pokemon, APPEARED, socketCliente);
		log_info(MANDATORY_LOGGER, "Se envio a TEAM un APPEARED_POKEMON, pokemon: '%s', posX: '%s', posY: '%s'.", list_get(pedidoGameBoy.argumentos, 0),
				list_get(pedidoGameBoy.argumentos, 1), list_get(pedidoGameBoy.argumentos, 2));
		break;
	default:
		log_error(MANDATORY_LOGGER, "Los parámetros ingresados no son compatibles con TEAM. Abortando.");
	}
}

ControladorGameBoy controladorTeam = { .proceso = TEAM, .atenderPedido = atenderPedidoTeam };
