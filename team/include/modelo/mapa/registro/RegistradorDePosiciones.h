//
// Created by Alan Zhao on 21/06/2020.
//

#ifndef TEAM_REGISTRADORDEPOSICIONES_H
#define TEAM_REGISTRADORDEPOSICIONES_H

#include "modelo/mapa/gps/Gps.h"
#include "modelo/equipo/entrenador/Entrenador.h"
#include "modelo/pokemon/PokemonAtrapable.h"

char * registrarEnMapaPosicionEntrenador(Mapa * mapa, Entrenador * posicionable);
char * registrarEnMapaPosicionPokemonAtrapable(Mapa * mapa, PokemonAtrapable * posicionable);

#endif //TEAM_REGISTRADORDEPOSICIONES_H
