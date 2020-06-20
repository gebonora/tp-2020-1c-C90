//
// Created by Alan Zhao on 30/05/2020.
//

#ifndef TEAM_MOVIMIENTO_H
#define TEAM_MOVIMIENTO_H

#include "delibird/protocol.h"

/**
 * Los entes del mapa deben soportar ser desplazados en 4 direcciones posibles.
 * Dura 1 ciclo de CPU c/u.
 */

typedef enum DireccionDesplazamiento {
    ARRIBA,
    ABAJO,
    IZQUIERDA,
    DERECHA
} DireccionDesplazamiento;

// Dudo que se tenga que resolver como un problema de path finding. Lloro sino.
// typedef t_queue * Camino;
// Camino generarCamino(Coordinate origen, Coordinate destino)

Coordinate moverEnDireccion(Coordinate origen, DireccionDesplazamiento direccion);

#endif //TEAM_MOVIMIENTO_H
