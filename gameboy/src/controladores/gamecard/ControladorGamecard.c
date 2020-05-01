//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/gamecard/ControladorGamecard.h"

void atenderPedidoGamecard(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    log_info(logger, "Se atendio el pedido en el controlador de GAMECARD");
    //TODO: Implementar atencion
}

ControladorGameBoy controladorGamecard = {.proceso=GAMECARD, .atenderPedido=atenderPedidoGamecard};