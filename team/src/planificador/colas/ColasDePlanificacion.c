//
// Created by Alan Zhao on 11/07/2020.
//

#include "planificador/colas/ColasDePlanificacion.h"

ColasDePlanificacion * crearColasDePlanificacion() {
    ColasDePlanificacion * colas = malloc(sizeof(ColasDePlanificacion));
    colas->colaNew = list_create();
    colas->colaBlocked = list_create();
    colas->colaExec = list_create();
    colas->colaReady = list_create();
    colas->colaExit = list_create();
    return colas;
}

void destruirColasDePlanificacion(ColasDePlanificacion * colasDePlanificacion, void (*destructor)(UnidadPlanificable *)) {
    list_destroy_and_destroy_elements(colasDePlanificacion->colaNew, (void (*)(void *)) destructor);
    free(colasDePlanificacion);
}
