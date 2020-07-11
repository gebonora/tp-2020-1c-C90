//
// Created by Alan Zhao on 17/06/2020.
//

#ifndef TEAM_EQUIPO_H
#define TEAM_EQUIPO_H

#include "modelo/equipo/entrenador/Entrenador.h"

typedef t_list * Equipo; // Un equipo es una lista de entrenadores

Equipo crearEquipoPorConfiguracion(); // Devuelve una lista de entrenadores en base a la config.
void destruirEquipo(Equipo equipo);

Equipo equipoProcesoTeam;

#endif //TEAM_EQUIPO_H
