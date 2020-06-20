//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_UNIDADPLANIFICABLE_H
#define TEAM_UNIDADPLANIFICABLE_H

#include "planificador/estados/EstadoPlanificador.h"

/**
 * Esta clase es la que va a mover el planificador. Se busca encapsular el dominio de lo planificable para que
 * los cambios en el [lo que sea que planifiquemos] no interfieran con el modelo de planificador.
 */

typedef struct UnidadPlanificable {
    int id;
    EstadoPlanificador estado;
    void* unidad;
    // Interfaz publica
    // actualizarEstado() : void - Cambia el estado interno y se lo notifica a la unidad.
} UnidadPlanificable;

#endif //TEAM_UNIDADPLANIFICABLE_H
