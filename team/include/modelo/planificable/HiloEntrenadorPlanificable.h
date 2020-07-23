//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_HILOENTRENADORPLANIFICABLE_H
#define TEAM_HILOENTRENADORPLANIFICABLE_H

#include <semaphore.h>
#include "app/Global.h"
#include "modelo/equipo/entrenador/Entrenador.h"
#include "delibird/utils/hilos/HiloFacil.h"
#include "modelo/planificable/tarea/TareaPlanificable.h"
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"

/**
 * La idea es que esta clase sea planificable, dando lugar a la ejecucion de las acciones propias del entrenador
 * dentro de un hilo que va a estar bloqueado por un semaforo hasta que el planificador lo mande a exec.
 * Aca se va a determinar COMO y CUANDO llamar a los metodos que exponga el entrenador.
 *
 * ¿Que cosas pueden mandarse a ejecutar?
 *      - Movimiento(entrenador, coordenada)
 *      - Intercambio(entrenador, coordenada, idEntrenador)
 *      - Captura(entrenador, coordenada, especiePokemon)
 *
 * IDEA: armar un metodo que permita el ingreso de trabajo/tarea. Por dentro vamos a asignarsela al planificable, yq que
 *  de esta forma compartimos memoria entre hilos. Luego vamos a ir haciendo signals de acuerdo a la cantidad
 *  de ciclos que nos pasen. Cada ciclo ejecutara un pasito de la tarea.
 *  Dejamos registro de la ultima ejecucion en un struct InfoUltimaEjecucion del planificable.
 */

typedef struct InfoUltimaEjecucion {
    double est_raf_ant;
    double real_raf_ant;
    double est_raf_actual;
    bool seEjecutoPrimeraEstimacion;
    double rafaga_real_actual;

    // parametros para sjf con desalojo}
    bool seNecesitaNuevaEstimacion;
    double rafaga_parcial_ejecutada; // lo que ejecuto para restarselo a la rafaga

} InfoUltimaEjecucion;

typedef struct HiloEntrenadorPlanificable {
    t_log * logger;
    Entrenador * entrenador;
    int retardoInstruccion;
    bool finDeTrabajo;
    sem_t semaforoEjecucionHabilitada;
    sem_t semaforoCicloCompletado;
    sem_t semaforoFinDeTrabajo;
    TareaPlanificable * tareaAsignada; //Nos sirve para que el hilo pueda levantar de aca la tarea que le dejen.
    InfoUltimaEjecucion infoUltimaEjecucion;
    // Interfaz publica
    void (*asignarTarea)(struct HiloEntrenadorPlanificable * this, TareaPlanificable * tarea);
    void (*trabajar)(struct HiloEntrenadorPlanificable * this);
    void (*ejecutarParcialmente)(struct HiloEntrenadorPlanificable * this, int cantInstrucciones);
    void (*ejecutar)(struct HiloEntrenadorPlanificable * this);
    // nuevoEstado(estado) - Asi nos enteramos si nos dan permiso para ejecutar o nos bloquearon TODO ¿Lo necesito?
    void (*destruir)(struct HiloEntrenadorPlanificable * this);
} HiloEntrenadorPlanificable;

extern const struct HiloEntrenadorPlanificableClass {
    HiloEntrenadorPlanificable *(*new)(Entrenador * entrenador);
} HiloEntrenadorPlanificableConstructor;

#endif //TEAM_HILOENTRENADORPLANIFICABLE_H
