//
// Created by Alan Zhao on 29/04/2020.
//

#include "utils/programadorDeTareas/ProgramadorDeTareas.h"

void programarFuncion(Tarea *tarea) {
    int milisegundosTranscurridos = 0;
    while (tarea->activo) {
        while (milisegundosTranscurridos < tarea->intervaloEnMilisegundos && tarea->activo) {
            usleep(10000);
            milisegundosTranscurridos += 10;
        }
        if (tarea->activo) {
            tarea->funcion(tarea->args);
        } else {
            break;
        }
        milisegundosTranscurridos = 0;
    }
}

void programarFuncionInfinita(Tarea *tarea) {
    while (tarea->activo) {
        tarea->funcion(tarea->args);
    }
}

void ejecutarTarea(Tarea *tarea) {
    if (tarea->esInfinita) {
        crearHilo(programarFuncionInfinita, tarea);
    } else {
        crearHilo(programarFuncion, tarea);
    }
}

void destruirTarea(Tarea *tarea) {
    tarea->activo = false;
    sleep(1);
    free(tarea);
}

Tarea *crearTarea(char *descripcion, void (*funcion)(void *), void *args, int intervaloEnMilisegundos) {
    Tarea *tarea = malloc(sizeof(Tarea));
    tarea->descripcion = descripcion;
    tarea->funcion = funcion;
    tarea->args = args;
    tarea->intervaloEnMilisegundos = intervaloEnMilisegundos;
    tarea->activo = true;
    tarea->esInfinita = intervaloEnMilisegundos == 0 ? true : false;
    return tarea;
}