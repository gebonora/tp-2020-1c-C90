//
// Created by Alan Zhao on 24/07/2020.
//

#ifndef TEAM_TAREADECAPTURA_H
#define TEAM_TAREADECAPTURA_H

#include "modelo/planificable/tarea/TareaPlanificable.h"
#include "modelo/equipo/Equipo.h"

TareaPlanificable * generarTareaDeIntercambio(Entrenador * entrenadorMovil, Entrenador * entrenadorEstatico, char * pokemonAEntregar, char * pokemonARecibir);

void intercambiarPokemones(Entrenador * entrenadorMovil, char * idEntrenadorEstatico, char * pokemonAEntrengar, char * pokemonARecibir);

#endif //TEAM_TAREADECAPTURA_H
