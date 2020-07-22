//
// Created by Alan Zhao on 20/07/2020.
//

#ifndef TEAM_CLIENTEBROKERV2_H
#define TEAM_CLIENTEBROKERV2_H

#include "app/Global.h"
#include "delibird/utils/sockets/Sockets.h"
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"

typedef struct RespuestaBroker {
    bool esValida;
    uint32_t idCorrelatividad;
} RespuestaBroker;

typedef struct ClienteBrokerV2 {
    t_log * logger;
    char * ipBroker;
    char * puertoBroker;
    RespuestaBroker (*enviarGet)(struct ClienteBrokerV2 * this, char* nombrePokemon);
    RespuestaBroker (*enviarCatch)(struct ClienteBrokerV2 * this, char * especie, uint32_t posX, uint32_t posY);
    void (*destruir)(struct ClienteBrokerV2 * this);
} ClienteBrokerV2;

extern const struct ClienteBrokerV2Class {
    ClienteBrokerV2 * (*new)();
} ClienteBrokerV2Constructor;

ClienteBrokerV2 * clienteBrokerV2ProcesoTeam;

#endif //TEAM_CLIENTEBROKERV2_H
