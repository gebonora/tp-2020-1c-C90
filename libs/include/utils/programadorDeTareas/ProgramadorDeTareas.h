//
// Created by Alan Zhao on 29/04/2020.
//

#ifndef DELIBIRD_PROGRAMADORDETAREAS_H
#define DELIBIRD_PROGRAMADORDETAREAS_H

#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils/hilos/HiloFacil.h"

typedef struct Tarea {
    char *descripcion;
    void (*funcion)(void *);
    void *args;
    int intervaloEnMilisegundos;
    bool activo;
    bool esInfinita;
} Tarea;

void ejecutarTarea(Tarea *tarea);

void programarFuncion(Tarea *tarea);

void programarFuncionInfinita(Tarea *tarea);

void destruirTarea(Tarea *tarea);

Tarea *crearTarea(char *descripcion, void (*funcion)(void *), void *args, int intervaloEnMilisegundos);

#endif //DELIBIRD_PROGRAMADORDETAREAS_H
