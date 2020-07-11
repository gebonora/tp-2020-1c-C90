//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_PLANIFICADOR_H
#define TEAM_PLANIFICADOR_H

#include "app/Global.h"
#include "planificador/transicionador/TransicionadorDeEstados.h"
#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"

/**
 * Encargado de darle lugar a los entrenadores para que ejecuten sus acciones 1 a la vez.
 * Los hilos de los entrenadores van a estar bloqueados por un semaforo, y el planificador les va a dar signal.
 * Grado de multiprogramacion 1.
 *
 * Problemas:
 *  - ¿Puedo no conocer las reglas del servicio de planificación para seguir trabajando con un estado avanzado?
 *      Por ejemplo, si 3 entrenadores necesitan ejecutar, voy mandando de a uno a exec y replanifico.
 *  - ¿Que es replanificar? ¿Cuando replanifico? Ver teoria.
 *      Cuando un entrenador sale de EXEC porque se le termino el quantum o porque hizo lo que tenia que hacer
 *      Corro de nuevo el algoritmo y selecciono al entrenador a planificado/mandar a EXEC.
 *  - ¿Se puede concebir una version sincrónica?
 *      Con el localized las tareas del planificador podrian terminar luego de haber bloqueado al entrenador.
 *      No porque el planificador va a estar trabajando aunqque no llegun eventos. Un intercambio por ej.
 *
 */

typedef struct Planificador {
    t_log *logger;
    AlgoritmoPlanificador algoritmoPlanificador;
    TransicionadorDeEstados transicionadorDeEstados;
    ColasDePlanificacion * colas;
    // Interfaz publica
    void (*agregarUnidadesPlanificables)(struct Planificador * planificador, t_list * unidadesPlanificables);
    void (*agregarUnidadPlanificable)(struct Planificador * planificador, UnidadPlanificable * unidadPlanificable); // Agrega una UP a la lista y le asigna el estado NEW
    // planificar(UP) - Manda una UP a EXEC.
    // finalizar(UP) - Manda la UP a EXIT, si no hay mas UP activas, logre el objetivo global.
    void (*destruir)(struct Planificador *this, void (*destructorUnidadPlanificable)(UnidadPlanificable *));
} Planificador;

extern const struct PlanificadorClass {
    Planificador (*new)();
} PlanificadorConstructor;

#endif //TEAM_PLANIFICADOR_H
