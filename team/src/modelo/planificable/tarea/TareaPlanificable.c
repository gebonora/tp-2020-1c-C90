//
// Created by Alan Zhao on 07/07/2020.
//

#include "modelo/planificable/tarea/TareaPlanificable.h"

static Instruccion * proximaInstruccion(TareaPlanificable * this) {
    this->estado = EJECUTANDO;
    return (Instruccion *) list_get(this->instrucciones, this->contadorDeInstrucciones);
}

static void actualizarEstado(TareaPlanificable * this, int ultimaInstruccionEjecutada) {
    if (this->estado == ABORTADA) {
        return;
    }
    if (this->contadorDeInstrucciones != ultimaInstruccionEjecutada) {
        // Orden de ejecución corrompido. Me notificaron que se ejecuto una instruccion que no me fue solicitada.
        this->estado = ABORTADA;
    } else if (this->totalInstrucciones == this->contadorDeInstrucciones + 1) {
        this->estado = FINALIZADA;
    } else {
        this->estado = PENDIENTE_DE_EJECUCION;
    }
}

static void notificarEjecucion(TareaPlanificable *this, int numeroInstruccion) {
    if (this->contadorDeInstrucciones == numeroInstruccion) {
        this->contadorDeInstrucciones+=1; //Avanzo solo si me notifican que la ultima que pidieron fue ejecutada.
    }
    actualizarEstado(this, numeroInstruccion);
}

static void destruir(TareaPlanificable * this) {
    log_destroy(this->logger);
    list_destroy_and_destroy_elements(this->instrucciones, (void (*)(void *)) destruirInstruccion);
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
    tarea->notificarEjecucion = &notificarEjecucion;
    tarea->destruir = &destruir;
    tarea->actualizarEstado = &actualizarEstado;

    return tarea;
}

const struct TareaPlanificableClass TareaPlanificableConstructor = {.new=&new};

char * nombreEstadoTareaPlanificable(EstadoTareaPlanificable estado) {
    switch (estado) {
        case PENDIENTE_DE_EJECUCION:
            return "PENDIENTE_DE_EJECUCION";
        case EJECUTANDO:
            return "EJECUTANDO";
        case FINALIZADA:
            return "FINALIZADA";
        case ABORTADA:
            return "ABORTADA";
        default:
            return "DESCONOCIDO";
    }
}

void destruirInstruccion(Instruccion * instruccion) {
    free(instruccion->descripcion);
    free(instruccion);
}