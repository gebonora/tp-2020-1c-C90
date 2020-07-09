//
// Created by Alan Zhao on 07/07/2020.
//

#include "modelo/mapa/movimiento/Movimiento.h"

Camino generarCamino(Coordinate origen, Coordinate destino) {
    return list_create(); //TODO
}

int distanciaEntre(Coordinate origen, Coordinate destino) {
    Camino camino = generarCamino(origen, destino);
    int distancia = list_size(camino);
    list_destroy(camino);
    return distancia;
}