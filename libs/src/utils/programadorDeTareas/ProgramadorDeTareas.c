//
// Created by Alan Zhao on 29/04/2020.
//

#include "utils/programadorDeTareas/ProgramadorDeTareas.h"

void programarFuncion(TareaEnSegundoPlano *tarea) {
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

void programarFuncionSinIntervalo(TareaEnSegundoPlano *tarea) {
    while (tarea->activo) {
        tarea->funcion(tarea->args);
    }
}

void ejecutarTareaEnSegundoPlano(TareaEnSegundoPlano *tarea) {
    if (tarea->intervaloEnMilisegundos == INTERVALO_NO_DEFINIDO) {
        crearHilo((void *(*)(void *)) programarFuncionSinIntervalo, tarea);
    } else {
        crearHilo((void *(*)(void *)) programarFuncion, tarea);
    }
}

void destruirTareaEnSegundoPlano(TareaEnSegundoPlano *tarea) {
    tarea->activo = false;
    sleep(1);
    free(tarea);
}

TareaEnSegundoPlano *crearTareaProgramableEnSegundoPlano(char *descripcion, void (*funcion)(void *), void *args, int intervaloEnMilisegundos) {
    TareaEnSegundoPlano *tarea = malloc(sizeof(TareaEnSegundoPlano));
    tarea->descripcion = descripcion;
    tarea->funcion = funcion;
    tarea->args = args;
    tarea->intervaloEnMilisegundos = intervaloEnMilisegundos;
    tarea->activo = true;
    return tarea;
}

TareaEnSegundoPlano *crearTareaPermanenteEnSegundoPlano(char *descripcion, void (*funcion)(void *), void *args) {
    return crearTareaProgramableEnSegundoPlano(descripcion, funcion, args, INTERVALO_NO_DEFINIDO);
}