//
// Created by Alan Zhao on 30/04/2020.
//

#ifndef GAMEBOY_CONTROLADORPEDIDOSGAMEBOY_H
#define GAMEBOY_CONTROLADORPEDIDOSGAMEBOY_H

#include "app/Global.h"
#include "modelo/PedidoGameBoy.h"
#include "modelo/controladorGameBoy/ControladorGameBoy.h"

typedef struct ControladorPedidosGameBoy {
    t_log *logger;
    void (*despacharPedido)(struct ControladorPedidosGameBoy *this, PedidoGameBoy pedidoGameBoy);
    void (*destruir)(struct ControladorPedidosGameBoy *this);
} ControladorPedidosGameBoy;

extern const struct ControladorPedidosGameBoyClass {
    ControladorPedidosGameBoy (*new)();
} ControladorPedidosGameBoyConstructor;

#endif //GAMEBOY_CONTROLADORPEDIDOSGAMEBOY_H
