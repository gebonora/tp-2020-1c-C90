//
// Created by Alan Zhao on 07/07/2020.
//

#include "modelo/mapa/movimiento/Movimiento.h"

Camino generarCamino(Coordinate origen, Coordinate destino) {
    Camino camino =  list_create();

    int distanciaX = destino.pos_x - origen.pos_x;
    int distanciaY = destino.pos_y - origen.pos_y;

    if (distanciaX == 0 && distanciaX == distanciaY) {
        return camino;
    }

    int modificadorSignoX = distanciaX > 0 ? 1 : -1;
    int modificadorSignoY = distanciaY > 0 ? 1 : -1;

    Coordinate * coordinate;

    for (int i = 1; i <= abs(distanciaX); i++) {
        coordinate = create_coordinate(origen.pos_x + (modificadorSignoX * i), origen.pos_y);
        list_add(camino, coordinate);
    }

    for (int i = 1; i <= abs(distanciaY); i++) {
        coordinate = create_coordinate(destino.pos_x, origen.pos_y + (modificadorSignoY * i));
        list_add(camino, coordinate);
    }

    return camino;
}

int distanciaEntre(Coordinate origen, Coordinate destino) {
    Camino camino = generarCamino(origen, destino);
    int distancia = list_size(camino);
    list_destroy_and_destroy_elements(camino, free);
    return distancia;
}