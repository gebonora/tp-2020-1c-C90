//
// Created by Alan Zhao on 29/05/2020.
//

#ifndef TEAM_SERVICIODEPLANIFICACION_H
#define TEAM_SERVICIODEPLANIFICACION_H

#include "app/Global.h"

/**
 * Esta clase es conocedora de que implicancias a nivel planificacion tienen los eventos del sistema.
 *
 * Problemas:
 *  - Alguien tiene que persistir los ids, ¿Sera que puedo modelar los eventos y guardarlos ahi?
 *      Modelar eventos esperables y registrarlos en el event-handler.
 *  - ¿Esto deberia soportar la posibilidad de procesamiento asincronico?
 *      Si. Vamos a tener que armar una cola de trabajo y correr en un hilo aparte.
 *  - Llego hasta tener al entrenador bloqueado esperando la respuesta del CATCH, ¿ya hago return void?
 *      Registro en el ManejadorDeEventos que necesito un CAUGHT y retorno.
 *
 * Responsabilidades:
 *  - Tiene conocimiento de las reglas de movimiento de los entrenadores en los diferentes estados.
 *  - Colaborar con la resolucion de deadlock facilitando mover entrenadores y realizar intercambios.
 */

typedef struct ServicioDePlanificacion {
    t_log * logger;
    t_queue * colaDeTrabajo;
    // Interfaz publica
    void (*trabajar)(struct ServicioDePlanificacion * this);
    void (*destruir)(struct ServicioDePlanificacion * this);
} ServicioDePlanificacion;

extern const struct ServicioDePlanificacionClass {
    ServicioDePlanificacion (*new)();
} ServicioDePlanificacionConstructor;

ServicioDePlanificacion servicioDePlanificacion;

#endif //TEAM_SERVICIODEPLANIFICACION_H
