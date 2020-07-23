//
// Created by Alan Zhao on 02/06/2020.
//

#ifndef TEAM_POKEMONATRAPABLE_H
#define TEAM_POKEMONATRAPABLE_H

#include <pthread.h>
#include "app/Global.h"
#include "modelo/mapa/gps/Gps.h"
#include "modelo/mapa/coordenadas/UtilidadesCoordenadas.h"

/**
 * Entidad clave en el proceso. Los dejamos en el mapa para que los entrenadores puedan capturarlos.
 */

typedef struct PokemonAtrapable {
    t_log * logger;
    char * especie;
    Coordinate posicionInicial;
    Gps * gps;
    pthread_mutex_t mutexAtrapable;
    TipoPosicionable tipoPosicionable;
    bool disponible;
    char * idMarcador;
    // Interfaz publica
    Posicion (*posicion)(struct PokemonAtrapable * this);
    bool (*marcarComoObjetivo)(struct PokemonAtrapable * this, char * idMarcador); //La idea es evitar que 2 entrenadores vayan a la misma posicion.
    bool (*esAtrapable)(struct PokemonAtrapable * this); //Es true si ningun entrenador lo marcó como objetivo
    bool (*borrarPosicion)(struct PokemonAtrapable * this);
    char * (*descripcion)(struct PokemonAtrapable * this);
    void (*destruir)(struct PokemonAtrapable * this);
} PokemonAtrapable;

extern const struct PokemonAtrapableClass {
    PokemonAtrapable * (*new)(char * especie, char * posicionInicial);
} PokemonAtrapableConstructor;

#endif //TEAM_POKEMONATRAPABLE_H
