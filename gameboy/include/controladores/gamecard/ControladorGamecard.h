//
// Created by Alan Zhao on 30/04/2020.
//

#ifndef GAMEBOY_CONTROLADORGAMECARD_H
#define GAMEBOY_CONTROLADORGAMECARD_H

#include "modelo/controladorGameBoy/ControladorGameBoy.h"
#include "delibird/utils/sockets/Sockets.h"
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"

void atenderPedidoGamecard(PedidoGameBoy pedidoGameBoy, t_log * logger);

#endif //GAMEBOY_CONTROLADORGAMECARD_H
