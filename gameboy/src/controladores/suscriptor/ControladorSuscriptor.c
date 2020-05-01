//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/suscriptor/ControladorSuscriptor.h"

void atenderPedidoSuscriptor(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    log_info(logger, "Se atendio el pedido en el controlador de SUSCRIPTOR");
    //TODO: Implementar atencion
}

ControladorGameBoy controladorSuscriptor = {.proceso=SUSCRIPTOR, .atenderPedido=atenderPedidoSuscriptor};