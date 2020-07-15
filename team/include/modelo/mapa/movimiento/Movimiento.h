//
// Created by Alan Zhao on 30/05/2020.
//

#ifndef TEAM_MOVIMIENTO_H
#define TEAM_MOVIMIENTO_H

#include "app/Global.h"
#include "delibird/protocol.h"

/**
 * Los entes del mapa deben soportar ser desplazados en 4 direcciones posibles.
 *
 * Algoritmo para generar movimientos.
 * Si quiero ir de (A,B) a (X,Y) -> calculo X-A=Dx, Y-B=Dy -> Obtengo (Dx, Dy).
 * Sabiendo (Dx, Dy) puedo determinar las cantidades de variaciones y su signo en las coordenadas.
 * Primero calculo las coordenadas del movimiento en X, luego en Y.
 *  - Si Dx > 0: Genero |Dx| cantidad de coordenadas, donde cada una incrementa en 1 el X del anterior.
 *  - Si Dx > 0: Genero |Dx| cantidad de coordenadas, donde cada una decrementa en 1 el X del anterior.
 *  - Si Dy > 0: Genero |Dy| cantidad de coordenadas, donde cada una incrementa en 1 el Y del anterior.
 *  - Si Dy < 0: Genero |Dy| cantidad de coordenadas, donde cada una decrementa en 1 el Y del anterior.
 *      Ej. (1,3) a (5,2) -> (Dx, Dy) = (4,-1)
 *          => [(2,3), (3,3), (4,3), (5,3), (5,2)] => Me toma 5 pasos llegar de (1,3) a (5,2), 4 a la derecha, 1 abajo.
 */

typedef t_list * Camino;

Camino generarCamino(Coordinate origen, Coordinate destino);
int distanciaEntre(Coordinate origen, Coordinate destino);

#endif //TEAM_MOVIMIENTO_H
