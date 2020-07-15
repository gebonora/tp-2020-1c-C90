//
// Created by Alan Zhao on 29/04/2020.
//

#ifndef DELIBIRD_PROGRAMADORDETAREAS_H
#define DELIBIRD_PROGRAMADORDETAREAS_H

#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils/hilos/HiloFacil.h"

#define INTERVALO_NO_DEFINIDO -777

typedef struct TareaEnSegundoPlano {
    char *descripcion;
    void (*funcion)(void *);
    void *args;
    int intervaloEnMilisegundos;
    bool activo;
} TareaEnSegundoPlano;

void ejecutarTareaEnSegundoPlano(TareaEnSegundoPlano *tarea);

void destruirTareaEnSegundoPlano(TareaEnSegundoPlano *tarea);

TareaEnSegundoPlano *crearTareaProgramableEnSegundoPlano(char *descripcion, void (*funcion)(void *), void *args, int intervaloEnMilisegundos);

TareaEnSegundoPlano *crearTareaPermanenteEnSegundoPlano(char *descripcion, void (*funcion)(void *), void *args);

#endif //DELIBIRD_PROGRAMADORDETAREAS_H
