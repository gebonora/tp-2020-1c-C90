//
// Created by Alan Zhao on 07/07/2020.
//

#ifndef TEAM_TAREAPLANIFICABLE_H
#define TEAM_TAREAPLANIFICABLE_H

#include "app/Global.h"

typedef enum EstadoTareaPlanificable {
    PENDIENTE_DE_EJECUCION,
    EJECUTANDO,
    FINALIZADA,
    ABORTADA
} EstadoTareaPlanificable;

typedef struct Instruccion {
    void (*funcion)(void * sujetoDeAplicacion, void * argumentos, ...);
    char * descripcion;
} Instruccion;

typedef t_list * Instrucciones;

typedef struct TareaPlanificable {
    t_log * logger;
    int contadorDeInstrucciones;
    int totalInstrucciones;
    Instrucciones instrucciones;
    EstadoTareaPlanificable estado;
    Instruccion * (*proximaInstruccion)(struct TareaPlanificable * this);
    void (*destruir)(struct TareaPlanificable * this);
} TareaPlanificable;

extern const struct TareaPlanificableClass {
    TareaPlanificable *(*new)(Instrucciones instrucciones);
} TareaPlanificableConstructor;

#endif //TEAM_TAREAPLANIFICABLE_H
