//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_PLANIFICADOR_H
#define TEAM_PLANIFICADOR_H

#include "app/Global.h"
#include "planificador/transicionador/TransicionadorDeEstados.h"
#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"
#include "servicios/servicioDeMetricas/ServicioDeMetricas.h"

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
	int quantum;
	AlgoritmoPlanificador algoritmoPlanificador;
	TransicionadorDeEstados transicionadorDeEstados;
	ColasDePlanificacion * colas;
	ServicioDeMetricas* servicioDeMetricas;
	// Interfaz publica
	void (*agregarUnidadesPlanificables)(struct Planificador * planificador, t_list * unidadesPlanificables); // Usada en el INIT del sistema.
	void (*agregarUnidadPlanificable)(struct Planificador * planificador, UnidadPlanificable * unidadPlanificable); // Agrega una UP a la lista y le asigna el estado NEW

	t_list* (*armarListaEntrenadoresDisponibles)(struct Planificador * planificador); // retorna cola NEW y BLOCKED sin espera.
	UnidadPlanificable* (*obtenerProximoAEjecutar)(struct Planificador * planificador);
	int (*cantidadDeRafagas)(struct Planificador * planificador, UnidadPlanificable * unidadPlanificable); // Nos sirve para saber cuanta mecha darle. Es dato.
	t_list* (*colaSegunEstado)(struct Planificador* this, EstadoPlanificador estado);
	void (*moverACola)(struct Planificador * this, UnidadPlanificable * uPlanificable, EstadoPlanificador estado, char* motivoCambio);
	EstadoPlanificador (*obtenerEstadoDeUnidadPlanificable)(struct Planificador* this, UnidadPlanificable* unidadPlanificable);
	void (*destruir)(struct Planificador *this, void (*destructorUnidadPlanificable)(UnidadPlanificable *));
} Planificador;

extern const struct PlanificadorClass {
	Planificador (*new)(ServicioDeMetricas* servicio);
} PlanificadorConstructor;

int minimo(int nro1, int nro2);
char* nombreDeLaCola(EstadoPlanificador estado);

#endif //TEAM_PLANIFICADOR_H
