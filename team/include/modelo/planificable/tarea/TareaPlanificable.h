//
// Created by Alan Zhao on 07/07/2020.
//

#ifndef TEAM_TAREAPLANIFICABLE_H
#define TEAM_TAREAPLANIFICABLE_H

#include "app/Global.h"
#include "modelo/equipo/entrenador/Entrenador.h"
#include "modelo/mapa/movimiento/Movimiento.h"
#include "modelo/mapa/Mapa.h"

typedef enum EstadoTareaPlanificable {
    PENDIENTE_DE_EJECUCION,
    EJECUTANDO,
    FINALIZADA,
    ABORTADA
} EstadoTareaPlanificable;

typedef struct Instruccion {
    int posicion;
    void (*funcion)(void * argumentos, ...);
    t_list * argumentos;
    char * descripcion;
} Instruccion;

typedef t_list * Instrucciones;

typedef struct TareaPlanificable {
    t_log * logger;
    int contadorDeInstrucciones;
    int totalInstrucciones;
    Instrucciones instrucciones;
    EstadoTareaPlanificable estado;
    //Interfaz publica
    Instruccion * (*proximaInstruccion)(struct TareaPlanificable * this);
    void (*notificarEjecucion)(struct TareaPlanificable * this, int numeroInstruccion);
    int (*cantidadInstruccionesRestantes)(struct TareaPlanificable * this);
    void (*abortar)(struct TareaPlanificable * this);
    int (*cantidadInstruccionesEjecutadas)(struct TareaPlanificable* this);
    void (*destruir)(struct TareaPlanificable * this);
    //Metodos privados
    void (*actualizarEstado)(struct TareaPlanificable * this, int ultimaInstruccionEjecutada);
} TareaPlanificable;

extern const struct TareaPlanificableClass {
    TareaPlanificable *(*new)(Instrucciones instrucciones);
} TareaPlanificableConstructor;

char * nombreEstadoTareaPlanificable(EstadoTareaPlanificable estado);
void ejecutarInstruccion(Instruccion * instruccion, void * sujetoDeAplicacion);
Instruccion * crearInstruccion(int posicion, void (*funcion)(void * argumentos, ...),char * descripcion, t_list * args);
void destruirInstruccion(Instruccion * instruccion);

#endif //TEAM_TAREAPLANIFICABLE_H
