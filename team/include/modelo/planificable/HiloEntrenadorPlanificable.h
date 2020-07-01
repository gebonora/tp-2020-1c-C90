//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_HILOENTRENADORPLANIFICABLE_H
#define TEAM_HILOENTRENADORPLANIFICABLE_H

#include <pthread.h>
#include "app/Global.h"
#include "modelo/equipo/entrenador/Entrenador.h"

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
 * TODO: Determinar si es necesario nuevoEstado()
 * Tengo que introducir el concepto de TAREA. TODO: ¿Quien la genera?
 */

typedef struct HiloEntrenadorPlanificable {
    t_log * logger;
    Entrenador *entrenador;
    pthread_mutex_t mutex;
    // Semaforos bloqueantes - Frenan el hilo hasta que el nuevo estado sea EXEC.

    // Interfaz publica
    // iniciar()
    void (*trabajar)(struct HiloEntrenadorPlanificable * this);
    // nuevoEstado(estado) - Asi nos enteramos si nos dan permiso para ejecutar o nos bloquearon
    void (*destruir)(struct HiloEntrenadorPlanificable * this);
} HiloEntrenadorPlanificable;

extern const struct HiloEntrenadorPlanificableClass {
    HiloEntrenadorPlanificable *(*new)(Entrenador * entrenador);
} HiloEntrenadorPlanificableConstructor;

#endif //TEAM_HILOENTRENADORPLANIFICABLE_H
