//
// Created by Alan Zhao on 07/07/2020.
//

#ifndef TEAM_TAREADECAPTURA_H
#define TEAM_TAREADECAPTURA_H

#include "app/Global.h"
#include "modelo/equipo/entrenador/Entrenador.h"
#include "modelo/planificable/tarea/TareaPlanificable.h"
#include "modelo/mapa/movimiento/Movimiento.h"
#include "modelo/mapa/Mapa.h"

TareaPlanificable * generarTareaDeCaptura(Entrenador * entrenador, char * pokemon, Coordinate posicionPokemon);

#endif //TEAM_TAREADECAPTURA_H
