//
// Created by Alan Zhao on 30/04/2020.
//

#ifndef GAMEBOY_CONTROLADORGAMEBOY_H
#define GAMEBOY_CONTROLADORGAMEBOY_H

#include "modelo/PedidoGameBoy.h"

typedef struct ControladorGameBoy {
    Proceso proceso;
    void (*atenderPedido)(PedidoGameBoy pedidoGameBoy, t_log * logger);
} ControladorGameBoy;

ControladorGameBoy obtenerControladorPara(PedidoGameBoy pedidoGameBoy, t_log * logger);

// Lista de controladores
ControladorGameBoy controladorTeam;
ControladorGameBoy controladorBroker;
ControladorGameBoy controladorGamecard;
ControladorGameBoy controladorSuscriptor;

#endif //GAMEBOY_CONTROLADORGAMEBOY_H
