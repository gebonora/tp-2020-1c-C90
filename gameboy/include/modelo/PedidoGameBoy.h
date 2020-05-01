//
// Created by Alan Zhao on 30/04/2020.
//

#ifndef GAMEBOY_PEDIDOGAMEBOY_H
#define GAMEBOY_PEDIDOGAMEBOY_H

#include <commons/string.h>
#include "stdlib.h"
#include "app/Global.h"

typedef t_list * Argumentos;

typedef enum {
    BROKER,
    TEAM,
    GAMECARD,
    SUSCRIPTOR
} Proceso;

typedef enum {
    NEW_POKEMON,
    APPEARED_POKEMON,
    CATCH_POKEMON,
    CAUGHT_POKEMON,
    GET_POKEMON,
    UNTYPED_MESSAGE
} TipoMensaje;

typedef struct {
    Proceso proceso;
    TipoMensaje tipoMensaje;
    Argumentos argumentos;
} PedidoGameBoy;

PedidoGameBoy crearPedidoGameBoy(int cantidadArugmentos, char** arrayArgumentos);

Proceso obtenerProceso(char * nombreProceso);

TipoMensaje obtenerTipoMensaje(char * tipoMensaje);

char * nombreProceso(Proceso proceso);

char * nombreTipoMensaje(TipoMensaje tipoMensaje);

void eliminarPedidoGameBoy(PedidoGameBoy *pedidoGameBoy);

#endif //GAMEBOY_PEDIDOGAMEBOY_H
