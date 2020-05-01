//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/team/ControladorTeam.h"

void atenderPedidoTeam(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    log_info(logger, "Se atendio el pedido en el controlador de TEAM");
    //TODO: Implementar atencion
}

ControladorGameBoy controladorTeam = {.proceso=TEAM, .atenderPedido=atenderPedidoTeam};