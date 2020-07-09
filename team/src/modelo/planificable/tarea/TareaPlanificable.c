//
// Created by Alan Zhao on 07/07/2020.
//

#include "modelo/planificable/tarea/TareaPlanificable.h"

static Instruccion * proximaInstruccion(TareaPlanificable * this) {
    return (Instruccion *) list_get(this->instrucciones, this->contadorDeInstrucciones);
}

static void destruir(TareaPlanificable * this) {
    log_destroy(this->logger);
    list_destroy(this->instrucciones);
    free(this);
}

static TareaPlanificable *new(Instrucciones instrucciones) {
    TareaPlanificable * tarea = malloc(sizeof(TareaPlanificable));

    tarea->logger = log_create(TEAM_INTERNAL_LOG_FILE, "TareaPlanificable", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
    tarea->contadorDeInstrucciones = 0;
    tarea->totalInstrucciones = list_size(instrucciones);
    tarea->instrucciones = instrucciones;
    tarea->estado = PENDIENTE_DE_EJECUCION;
    tarea->proximaInstruccion = &proximaInstruccion;
    tarea->destruir = &destruir;

    return tarea;
}

const struct TareaPlanificableClass TareaPlanificableConstructor = {.new=&new};