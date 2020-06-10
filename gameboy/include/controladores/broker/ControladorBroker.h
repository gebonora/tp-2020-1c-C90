//
// Created by Alan Zhao on 30/04/2020.
//

#ifndef GAMEBOY_CONTROLADORBROKER_H
#define GAMEBOY_CONTROLADORBROKER_H

#include "modelo/controladorGameBoy/ControladorGameBoy.h"

void atenderPedidoBroker(PedidoGameBoy pedidoGameBoy, t_log * logger);
Operation get_operation(char*);

#endif //GAMEBOY_CONTROLADORBROKER_H
