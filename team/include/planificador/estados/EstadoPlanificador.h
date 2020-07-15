//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_ESTADOPLANIFICADOR_H
#define TEAM_ESTADOPLANIFICADOR_H

/**
 * Cuando se planifique un hilo de entrenador, se le sera asignado uno de estos estados
 */

typedef enum EstadoPlanificador {
    NEW_,
    READY,
    EXEC,
    BLOCK,
    EXIT
} EstadoPlanificador;

#endif //TEAM_ESTADOPLANIFICADOR_H
