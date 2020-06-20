//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_INTERCAMBIOPOKEMON_H
#define TEAM_INTERCAMBIOPOKEMON_H

#include "modelo/equipo/entrenador/Entrenador.h"

/**
 * Esta funcion se encargara de establecer las reglas para el intercambio de pokemones entre 2 entrenadores.
 * Un intercambio involucra unicamente 2 pokemones. Esto puede terminar siendo responsabilidad del entrenador.
 * Dura 5 ciclos de CPU c/u.
 */

void intercambiarPokemones(Entrenador entrenador1, Entrenador entrenador2);

#endif //TEAM_INTERCAMBIOPOKEMON_H
