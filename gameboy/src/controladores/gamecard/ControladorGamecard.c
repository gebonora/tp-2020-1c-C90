//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/gamecard/ControladorGamecard.h"

void atenderPedidoGamecard(PedidoGameBoy pedidoGameBoy, t_log * logger) {
	log_info(logger, "Se atendio el pedido en el controlador de GAMECARD");
	//TODO: Implementar atencion
	char* ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion,
	IP_GAMECARD);
	char* puerto = servicioDeConfiguracion.obtenerString(
			&servicioDeConfiguracion, PUERTO_GAMECARD);
	int socketCliente = crearSocketCliente(IP_GAMECARD, PUERTO_GAMECARD);

	switch (pedidoGameBoy.tipoMensaje) {
	case NEW_POKEMON:
		//implementar
		break;

	case CATCH_POKEMON:
		//implementar
		break;

	case GET_POKEMON:
		//implementar
		break;

	}

}

ControladorGameBoy controladorGamecard = { .proceso = GAMECARD, .atenderPedido =
		atenderPedidoGamecard };
