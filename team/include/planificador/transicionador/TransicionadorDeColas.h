//
// Created by Alan Zhao on 28/05/2020.
//

#ifndef TEAM_TRANSICIONADORDECOLAS_H
#define TEAM_TRANSICIONADORDECOLAS_H

#include "planificador/colas/ColasDePlanificacion.h"

/**
 * Encargado de mover una UP entre las colas, con la ayuda del transicionador de estados.
 */

typedef struct TransicionadorDeColas {
    ColasDePlanificacion colasDePlanificacion;
} TransicionadorDeColas;

#endif //TEAM_TRANSICIONADORDECOLAS_H
