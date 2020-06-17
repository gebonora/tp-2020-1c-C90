//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_ENTRENADOR_H
#define TEAM_ENTRENADOR_H


#include "app/Global.h"
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"
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
    Entrenador *(*new)(char * posicionInicial);
} EntrenadorConstructor;

Coordinate parsearPosicion(char * posicion); // Toma '1|2' y devuelve las coordenadas (1,2)

#endif //TEAM_ENTRENADOR_H
