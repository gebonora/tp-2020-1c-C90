//
// Created by Alan Zhao on 15/07/2020.
//

#ifndef TEAM_CAPTURAPOKEMON_H
#define TEAM_CAPTURAPOKEMON_H

#include "app/Global.h"
#include "modelo/pokemon/PokemonAtrapable.h"

typedef struct CapturaPokemon {
    t_log * logger;
    char * idEntrenador;
    char * idCorrelatividad;
    PokemonAtrapable * pokemonAtrapable;
    bool (*eliminarPokemonCapturadoDelMapa)(struct CapturaPokemon * this, Mapa mapa);
    void (*destruir)(struct CapturaPokemon * this);
} CapturaPokemon;

extern const struct CapturaPokemonClass {
    CapturaPokemon * (*new)(char * idEntrenador, char * idCorrelatividad, PokemonAtrapable * pokemon);
} CapturaPokemonConstructor;

#endif //TEAM_CAPTURAPOKEMON_H
