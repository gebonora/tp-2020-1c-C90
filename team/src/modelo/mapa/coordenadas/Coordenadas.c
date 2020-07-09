//
// Created by Alan Zhao on 25/06/2020.
//

#include "modelo/mapa/coordenadas/UtilidadesCoordenadas.h"

char * coordenadaClave(Coordinate posicion) {
    return string_from_format("(%u,%u)", posicion.pos_x, posicion.pos_y);
}

char * coordenadaPuntero(Coordinate * posicion) {
    return coordenadaClave(convertirACoordenada(posicion));
}

Coordinate convertirACoordenada(Coordinate * coordenadaPuntero) {
    Coordinate coordenada = {.pos_x = coordenadaPuntero->pos_x, .pos_y = coordenadaPuntero->pos_y};
    return coordenada;
}

Coordinate parsearPosicion(char * posicion, char * separador) {
    char ** punto = string_n_split(posicion, 2, separador);
    Coordinate coordenada = {.pos_x=atoi(punto[0]), .pos_y=atoi(punto[1])};

    free(punto[0]);
    free(punto[1]);
    free(punto);

    return coordenada;
}

Coordinate convertirClaveACoordenada(char * coordenadaImprimible) {
    char * posicion = string_substring(coordenadaImprimible, 1, string_length(coordenadaImprimible) - 2);
    Coordinate coordenada = parsearPosicion(posicion, ",");
    free(posicion);
    return coordenada;
}