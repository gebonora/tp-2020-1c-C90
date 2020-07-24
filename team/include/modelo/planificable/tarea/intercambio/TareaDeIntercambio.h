//
// Created by Alan Zhao on 24/07/2020.
//

#ifndef TEAM_TAREADECAPTURA_H
#define TEAM_TAREADECAPTURA_H

#include "app/Global.h"
#include "modelo/equipo/entrenador/Entrenador.h"
#include "modelo/planificable/tarea/TareaPlanificable.h"
#include "modelo/mapa/movimiento/Movimiento.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/pokemon/CapturaPokemon.h"
#include "modelo/pokemon/PokemonAtrapable.h"

TareaPlanificable * generarTareaDeIntercambio(Entrenador * entrenadorMovil, Entrenador * entrenadorEstatico, char * pokemonAEntregar, char * pokemonARecibir);

void intercambiarPokemones(Entrenador * entrenadorMovil, char * idEntrenadorEstatico, char * pokemonAEntrengar, char * pokemonARecibir);

#endif //TEAM_TAREADECAPTURA_H
