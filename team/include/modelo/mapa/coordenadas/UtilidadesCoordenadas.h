//
// Created by Alan Zhao on 25/06/2020.
//

#ifndef TEAM_UTILIDADESCOORDENADAS_H
#define TEAM_UTILIDADESCOORDENADAS_H

#include "app/Global.h"

char * armarCoordenadaClave(int x, int y);
char * coordenadaClave(Coordinate posicion); // Toma Coordinate => '(1,2)' que funciona como clave del mapa/plano.
char * coordenadaPuntero(Coordinate * posicion);
Coordinate convertirACoordenada(Coordinate * coordenadaPuntero);
Coordinate convertirClaveACoordenada(char * coordenadaImprimible); // Toma '(1,2)' y devuelve Coordinate
Coordinate parsearPosicion(char * coordenadaImprimible, char * separador);  // Toma '1|2' y separador => Coordinate
Coordinate * convertirAPunteroDeCoordenada(Coordinate posicion);
char * separarCoordenada(Coordinate posicion, char * separador);
char * separarPunteroCoordenada(Coordinate * posicion, char * separador);

#endif //TEAM_UTILIDADESCOORDENADAS_H
