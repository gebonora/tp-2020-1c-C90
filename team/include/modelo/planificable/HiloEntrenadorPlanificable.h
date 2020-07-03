//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_HILOENTRENADORPLANIFICABLE_H
#define TEAM_HILOENTRENADORPLANIFICABLE_H

#include <semaphore.h>
#include "app/Global.h"
#include "modelo/equipo/entrenador/Entrenador.h"
#include "delibird/utils/hilos/HiloFacil.h"

/**
 * La idea es que esta clase sea planificable, dando lugar a la ejecucion de las acciones propias del entrenador
 * dentro de un hilo que va a estar bloqueado por un semaforo hasta que el planificador lo mande a exec.
 * Aca se va a determinar COMO y CUANDO llamar a los metodos que exponga el entrenador.
 *
 * ¿Que cosas pueden mandarse a ejecutar?
 *      - Movimiento
 *      - Intercambio
 *      - Captura
 *
 * Tengo que introducir el concepto de TAREA. TODO: ¿Quien la genera?
 *      - El planificable va a tener un atributo TAREA. El servicioDePlanificacion le asignara una.
 *          Cuando termine, el planificable se encargara de destruirla para que puedan asignarle otra.
 * // Necesito que el hilo este pausado por un semaforo.
 * // El signal sobre ese semaforo me da pie a procesar de a poco la tarea.
 * // La cantidad de pasos a ejecutar de la tarea nos lo pasan como .....TODO
 * Modelo loop:
 *      While(finDeTrabajo) { // Cuando se liberen los recursos, esto se pone true y se sale.
 *          wait(semaforoEjecucionHabilitada); // El servicio de planificacion le da el ok para que el bucle gire.
 *
 *          TODO
 *          proximoPaso = tarea.proximoPaso() // Internamente hay un IP que se actualiza cuando le piden el paso.
 *
 *
 *      }
 */

typedef struct HiloEntrenadorPlanificable {
    t_log * logger;
    Entrenador *entrenador;
    bool finDeTrabajo;
    sem_t semaforoEjecucionHabilitada;
    sem_t semaforoFinDeTrabajo;

    // Interfaz publica
    void (*trabajar)(struct HiloEntrenadorPlanificable * this);
    // nuevoEstado(estado) - Asi nos enteramos si nos dan permiso para ejecutar o nos bloquearon TODO ¿Lo necesito?
    void (*destruir)(struct HiloEntrenadorPlanificable * this);
} HiloEntrenadorPlanificable;

extern const struct HiloEntrenadorPlanificableClass {
    HiloEntrenadorPlanificable *(*new)(Entrenador * entrenador);
} HiloEntrenadorPlanificableConstructor;

#endif //TEAM_HILOENTRENADORPLANIFICABLE_H
