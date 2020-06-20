//
// Created by Alan Zhao on 28/05/2020.
//

#ifndef TEAM_DISTANCIA_H
#define TEAM_DISTANCIA_H

#include "delibird/protocol.h"

/**
 * Cálculo de la distancia a un punto en el mapa. Necesario para seleccionar entrenadores.
 * Este criterio es importante puesto que determina cual de TODOS los candidatos pasa a READY.
 */

int distanciaA(Coordinate origen, Coordinate fin);

#endif //TEAM_DISTANCIA_H
