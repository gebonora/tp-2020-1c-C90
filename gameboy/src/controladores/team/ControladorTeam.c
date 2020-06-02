//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/team/ControladorTeam.h"

void atenderPedidoTeam(PedidoGameBoy pedidoGameBoy, t_log * logger) {
	log_info(logger, "Se atendio el pedido en el controlador de TEAM");
	//TODO: Implementar atencion
	char* ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion,
	IP_TEAM);
	char* puerto = servicioDeConfiguracion.obtenerString(
			&servicioDeConfiguracion, PUERTO_TEAM);
	int socketCliente = crearSocketCliente(IP_TEAM, PUERTO_TEAM);

	switch (pedidoGameBoy.tipoMensaje) {
	case APPEARED_POKEMON:
		//implementar
		break;

	}
}

ControladorGameBoy controladorTeam = { .proceso = TEAM, .atenderPedido =
		atenderPedidoTeam };
