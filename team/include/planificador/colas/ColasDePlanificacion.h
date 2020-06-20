//
// Created by Alan Zhao on 28/05/2020.
//

#ifndef TEAM_COLASDEPLANIFICACION_H
#define TEAM_COLASDEPLANIFICACION_H

#include <commons/collections/list.h>

typedef struct ColasDePlanificacion {
    t_list * colaNew;
    t_list * colaReady;
    t_list * colaExec;
    t_list * colaBlocked;
    t_list * colaExit;
} ColasDePlanificacion ;

#endif //TEAM_COLASDEPLANIFICACION_H
