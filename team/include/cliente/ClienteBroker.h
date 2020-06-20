//
// Created by Alan Zhao on 10/06/2020.
//

#ifndef TEAM_CLIENTEBROKER_H
#define TEAM_CLIENTEBROKER_H

/**
 * Idealmente, los mensajes que quiera enviarle al broker van a salir por aca.
 * Esta clase nos expone las formas de dejar un mensaje en el broker.
 * Ejemplo: Quiero atrapar a un pokemon, necesito enviar un CATCH.
 * Probablemente se pueda generalizar y mandar a libs.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include "app/Global.h"
#include "utils/sockets/Sockets.h"

void atenderCaughtRecibido(Caught* unCaught, uint32_t);

#endif //TEAM_CLIENTEBROKER_H
