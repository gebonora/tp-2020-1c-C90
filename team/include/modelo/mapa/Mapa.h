//
// Created by Alan Zhao on 30/05/2020.
//

#ifndef TEAM_MAPA_H
#define TEAM_MAPA_H

#include "app/Global.h"

/**
 * Aca llevamos cuenta de la posicion de los entes del tp que utilicen coordenadas.
 * Hasta donde sabemos, tanto entrenadores como pokemones tienen una posicion (x,y) asignada.
 * Cumple la funcion de ser el lugar para depositar pokemones xd.
 * El mapa es responsable de setearle el (x,y) a las cosas despues de moverlas en su memoria.
 */

typedef t_dictionary * Plano ;

typedef struct Mapa {
    t_log * logger;
    Plano plano;
    // Interfaz publica
    // hayPokemonesAtrapables() : bool - Es true si hay pokemones para atrapar.
    //char * (*registrarPosicion)(Coordinate posicionInicial);
    void (*destruir)(struct Mapa * this);
} Mapa;

extern const struct MapaClass {
    Mapa (*new)();
} MapaConstructor;

Mapa mapa;

#endif //TEAM_MAPA_H
