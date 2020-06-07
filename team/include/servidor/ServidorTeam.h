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
#include "app/Servicios.h"

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
