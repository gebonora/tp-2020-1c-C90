//
// Created by Alan Zhao on 02/06/2020.
//

#ifndef TEAM_POKEMONATRAPABLE_H
#define TEAM_POKEMONATRAPABLE_H

#include "delibird/interface.h"

/**
 * Basicamente es un wrapper para que podamos darle comportamiento al modelo pokemon de libs.
 * TODO: Ver si es posible que 2 entrenadores del team vayan a capturar al mismo pokemon.
 */

typedef struct PokemonAtrapable {
    Pokemon pokemon;
    // Interfaz publica
    // marcarComoObjetivo() : void - La idea es evitar que 2 entrenadores vayan a la misma posicion.
    // esAtrapable() : bool - Es true si ningun entrenador lo marcó como objetivo
} PokemonAtrapable;

#endif //TEAM_POKEMONATRAPABLE_H
