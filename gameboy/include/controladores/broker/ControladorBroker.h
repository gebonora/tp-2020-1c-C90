//
// Created by Alan Zhao on 30/04/2020.
//

#ifndef GAMEBOY_CONTROLADORBROKER_H
#define GAMEBOY_CONTROLADORBROKER_H

#include "modelo/controladorGameBoy/ControladorGameBoy.h"
#include <pthread.h>

typedef struct {
	t_log* logger;
	int socket;
	Operation operation;
} socket_with_logger;

void atenderPedidoBroker(PedidoGameBoy pedidoGameBoy, t_log * logger);
Operation get_operation(char*);
void listen_messages(socket_with_logger* swl);

#endif //GAMEBOY_CONTROLADORBROKER_H
