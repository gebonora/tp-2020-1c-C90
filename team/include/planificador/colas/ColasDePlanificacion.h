//
// Created by Alan Zhao on 28/05/2020.
//

#ifndef TEAM_COLASDEPLANIFICACION_H
#define TEAM_COLASDEPLANIFICACION_H

#include "app/Global.h"
#include <commons/collections/list.h>
#include "planificador/algoritmos/AlgoritmoPlanificador.h"

typedef struct ColasDePlanificacion {
    t_list * colaNew;
    t_list * colaReady;
    t_list * colaExec;
    t_list * colaBlocked;
    t_list * colaExit;
} ColasDePlanificacion ;

ColasDePlanificacion * crearColasDePlanificacion();
void destruirColasDePlanificacion(ColasDePlanificacion * colasDePlanificacion, void (*destructor)(UnidadPlanificable *));

#endif //TEAM_COLASDEPLANIFICACION_H
