//
// Created by Alan Zhao on 25/06/2020.
//

#ifndef TEAM_UTILIDADESCOORDENADAS_H
#define TEAM_UTILIDADESCOORDENADAS_H

#include "app/Global.h"

char * coordenadaClave(Coordinate posicion); // Toma Coordinate => '(1,2)' que funciona como clave del mapa/plano.
Coordinate convertirClaveACoordenada(char * coordenadaImprimible); // Toma '(1,2)' y devuelve Coordinate
Coordinate parsearPosicion(char * coordenadaImprimible, char * separador);  // Toma '1|2' y separador => Coordinate

#endif //TEAM_UTILIDADESCOORDENADAS_H
