//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_FIRSTINFIRSTOUT_H
#define TEAM_FIRSTINFIRSTOUT_H

#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "modelo/planificable/HiloEntrenadorPlanificable.h"

extern const struct FirstInFirstOutClass {
    AlgoritmoPlanificador (*new)();
} FirstInFirstOutConstructor;

#endif //TEAM_FIRSTINFIRSTOUT_H
