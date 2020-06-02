//
// Created by Alan Zhao on 30/04/2020.
//

#ifndef GAMEBOY_CONTROLADORTEAM_H
#define GAMEBOY_CONTROLADORTEAM_H

#include "modelo/controladorGameBoy/ControladorGameBoy.h"
#include "delibird/utils/sockets/Sockets.h"
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"

void atenderPedidoTeam(PedidoGameBoy pedidoGameBoy, t_log * logger);

#endif //GAMEBOY_CONTROLADORTEAM_H
