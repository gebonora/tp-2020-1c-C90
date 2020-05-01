//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/ControladorPedidosGameBoy.h"

static void despacharPedido(ControladorPedidosGameBoy *this, PedidoGameBoy pedidoGameBoy) {
    ControladorGameBoy controladorGameBoy = obtenerControladorPara(pedidoGameBoy, this->logger);
    log_debug(this->logger,
            "Se despacha al controlador de %s el mensaje %s con %d argumentos:",
            nombreProceso(pedidoGameBoy.proceso),
            nombreTipoMensaje(pedidoGameBoy.tipoMensaje),
            list_size(pedidoGameBoy.argumentos));
    for (int i=0; i<list_size(pedidoGameBoy.argumentos); i++) {
        log_debug(this->logger,"%d. %s",i + 1 , list_get(pedidoGameBoy.argumentos, i));
    }
    return controladorGameBoy.atenderPedido(pedidoGameBoy, this->logger);
}

static void destruir(ControladorPedidosGameBoy *this) {
    log_destroy(this->logger);
}

static ControladorPedidosGameBoy new() {
    return (ControladorPedidosGameBoy) {
            .logger = log_create(GAMEBOY_INTERNAL_LOG_FILE, "ControladorPedidosGameBoy", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_DEBUG),
            &despacharPedido,
            &destruir
    };
}

const struct ControladorPedidosGameBoyClass ControladorPedidosGameBoyConstructor = {.new = &new};

