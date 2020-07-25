//
// Created by Alan Zhao on 30/04/2020.
//

#ifndef GAMEBOY_CONTROLADORGAMEBOY_H
#define GAMEBOY_CONTROLADORGAMEBOY_H

#include "modelo/PedidoGameBoy.h"
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <delibird/protocol.h>
#include <delibird/interface.h>


typedef struct ControladorGameBoy {
    Process proceso;
    void (*atenderPedido)(PedidoGameBoy pedidoGameBoy, t_log * logger);
} ControladorGameBoy;

ControladorGameBoy obtenerControladorPara(PedidoGameBoy pedidoGameBoy, t_log * logger);

// Lista de controladores
ControladorGameBoy controladorTeam;
ControladorGameBoy controladorBroker;
ControladorGameBoy controladorGamecard;

#endif //GAMEBOY_CONTROLADORGAMEBOY_H
