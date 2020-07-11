#ifndef TEAM_SHORTESTJOBFIRST_H
#define TEAM_SHORTESTJOBFIRST_H

#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "modelo/planificable/HiloEntrenadorPlanificable.h"



double alpha;
double estInicial;

extern const struct ShortestJobFirstClass {
    AlgoritmoPlanificador (*new)();
} ShortestJobFirstConstructor;


#endif //TEAM_SHORTESTJOBFIRST_H
