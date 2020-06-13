//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_ALGORITMOPLANIFICADOR_H
#define TEAM_ALGORITMOPLANIFICADOR_H

#include "app/Global.h"
#include "modelo/planificable/HiloEntrenadorPlanificable.h"

/**
 * Bajo esta interfaz vamos a intentar crear cada posible algoritmo de planificacion.
 * Es necesario que respeten esta abstraccion para que el planificador pueda seleccionar
 * la implementacion especificada en el archivo de configuracion (ver Strategy pattern).
 * Como hay casos donde varios entrenadores califican al READY, es el algoritmo quien determina el pase a EXEC.
 */

typedef HiloEntrenadorPlanificable UnidadPlanificable;

typedef enum TipoAlgorimoPlanificador {
    FIFO,
    RR,
    SJF_CD,
    SJF_SD
} TipoAlgorimoPlanificador ;

typedef struct AlgoritmoPlanificador {
    t_log *logger;
    TipoAlgorimoPlanificador tipo;
    // Interfaz publica
    UnidadPlanificable *(*proximoAEjecutar)(struct AlgoritmoPlanificador *this, t_list * listaReady);
    void (*destruir)(struct AlgoritmoPlanificador *this);
} AlgoritmoPlanificador;

// Se va a llamar en la inicializacion del planificador
AlgoritmoPlanificador obtenerAlgoritmo(char * nombreAlgoritmo);

// Al levantar la app necesitamos cargar la lista de algoritmos
void inicializarAlgoritmosDePlanificacion();

// Lista de algoritmos
AlgoritmoPlanificador firstInFirstOut;
AlgoritmoPlanificador roundRobin;
AlgoritmoPlanificador shortestJobFirst; // Por defecto es sin desalojo
AlgoritmoPlanificador shortestRemainingTimeFirst; // Esta es otra forma de llamar al SJF con desalojo

#endif //TEAM_ALGORITMOPLANIFICADOR_H
