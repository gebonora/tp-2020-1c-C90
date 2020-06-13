//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_ENTRENADOR_H
#define TEAM_ENTRENADOR_H

#include "app/Global.h"
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include "delibird/interface.h"

/**
 * El entrenador se mueve y caza pokemones. La idea es llevar ese registro acá.
 */

typedef struct Entrenador {
    t_log * logger;
    Coordinate posicionInicial;
    t_dictionary* pokemones_capturados;
    t_dictionary* pokemones_objetivo;
    // limiteDeCaptura : int // Establece cuantos pokemones puede capturar

    // Interfaz publica
    // objetivoCompletado() : Bool - Es true si pokemones_objetivo es igual a pokemones_capturados.
    // puedeAtraparPokemones(): int - Es true si es < a limiteDeCaptura.
    void (*destruir)(struct Entrenador * this);
} Entrenador;

extern const struct EntrenadorClass {
    Entrenador *(*new)();
} EntrenadorConstructor;


typedef t_list * Equipo; // Un equipo es una lista de entrenadores

Equipo crearEquipoPorConfiguracion(); // Devuelve una lista de entrenadores en base a la config.

#endif //TEAM_ENTRENADOR_H
