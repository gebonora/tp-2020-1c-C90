//
// Created by Alan Zhao on 07/07/2020.
//

#ifndef TEAM_TAREAPLANIFICABLE_H
#define TEAM_TAREAPLANIFICABLE_H

#include "app/Global.h"

typedef struct TareaPlanificable {
    t_log * logger;
    int contadorDeInstrucciones;
    void (*destruir)(struct TareaPlanificable * this);
} TareaPlanificable;

extern const struct TareaPlanificableClass {
    TareaPlanificable *(*new)();
} TareaPlanificableConstructor;

#endif //TEAM_TAREAPLANIFICABLE_H
