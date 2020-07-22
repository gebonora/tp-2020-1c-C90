#ifndef TEAM_ROUNDROBIN_H
#define TEAM_ROUNDROBIN_H

#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "modelo/planificable/HiloEntrenadorPlanificable.h"

int quantum;

void obtenerConfiguracionesRR();

extern const struct RoundRobinClass {
    AlgoritmoPlanificador (*new)();
} RoundRobinConstructor;

#endif //TEAM_ROUNDROBIN_H
