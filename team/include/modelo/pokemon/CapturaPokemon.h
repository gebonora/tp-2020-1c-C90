//
// Created by Alan Zhao on 15/07/2020.
//

#ifndef TEAM_CAPTURAPOKEMON_H
#define TEAM_CAPTURAPOKEMON_H

#include "app/Global.h"
#include "modelo/pokemon/PokemonAtrapable.h"
#include "modelo/equipo/entrenador/Entrenador.h"

typedef struct CapturaPokemon {
    t_log * logger;
    Entrenador * entrenador;
    uint32_t idCorrelatividad;
    PokemonAtrapable * pokemonAtrapable;
    bool (*eliminarPokemonCapturadoDelMapa)(struct CapturaPokemon * this, Mapa mapa);
    char * (*posicion)(struct CapturaPokemon * this);
    char * (*especie)(struct CapturaPokemon * this);
    char * (*idEntrenador)(struct CapturaPokemon * this);
    void (*destruir)(struct CapturaPokemon * this);
} CapturaPokemon;

extern const struct CapturaPokemonClass {
    CapturaPokemon * (*new)(Entrenador * entrenador, uint32_t idCorrelatividad, PokemonAtrapable * pokemon);
} CapturaPokemonConstructor;

#endif //TEAM_CAPTURAPOKEMON_H
