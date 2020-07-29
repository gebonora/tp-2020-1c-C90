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
    list_destroy_and_destroy_elements(colasDePlanificacion->colaExec, (void (*)(void *)) destructor);
    list_destroy_and_destroy_elements(colasDePlanificacion->colaExit, (void (*)(void *)) destructor);
    list_destroy_and_destroy_elements(colasDePlanificacion->colaBlocked, (void (*)(void *)) destructor);
    list_destroy_and_destroy_elements(colasDePlanificacion->colaReady, (void (*)(void *)) destructor);
    free(colasDePlanificacion);
}
