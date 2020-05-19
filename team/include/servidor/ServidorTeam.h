//
// Created by Alan Zhao on 07/05/2020.
//

#ifndef TEAM_SERVIDORTEAM_H
#define TEAM_SERVIDORTEAM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "app/Global.h"
#include "servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"

#include "libs/include/protocol.h"

void atenderAppeared();
void esperarBrokerAppeared(int socketDeEscucha);
void procesarHiloAppeared(Pokemon* unPokemon);

void atenderCaught();
void esperarBrokerCaught(int socketDeEscucha);
void procesarHiloCaught(Caught* unCaught);

void atenderLocalized();
void esperarBrokerLocalized(int socketDeEscucha);
void procesarHiloLocalized(Localized* unLocalized);

int iniciarSocketDeEscucha() ;
int crearSocketHaciaBroker();

void atenderGameboy();


typedef struct ServidorTeam {
    int socketServidor;
    t_log *logger;
    void (*iniciar)(struct ServidorTeam *this);
    void (*atenderConexiones)(struct ServidorTeam *this);
    void (*terminar)(struct ServidorTeam *this);
} ServidorTeam;

extern const struct ServidorTeamClass {
    ServidorTeam (*new)();
} ServidorTeamConstructor;

ServidorTeam servidorTeam;

#endif //TEAM_SERVIDORTEAM_H
