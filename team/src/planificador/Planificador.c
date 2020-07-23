//
// Created by Alan Zhao on 11/07/2020.
//

#include "planificador/Planificador.h"

void agregarUnidadesPlanificables(Planificador * this, t_list * unidadesPlanificables) {
    list_add_all(this->colas->colaNew, unidadesPlanificables);
}

void agregarUnidadPlanificable(Planificador * this, UnidadPlanificable * unidadPlanificable) {
    list_add(this->colas->colaNew, unidadPlanificable);
}

t_list* armarListaEntrenadoresDisponibles (Planificador * this){
	// retorna NEW y BLOCKED activo
	return NULL;
}

UnidadPlanificable* obtenerProximoAEjecutar(Planificador * this){
	// correr algoritmo
	return NULL;
}

int cantidadDeRafagas(Planificador * planificador, UnidadPlanificable * unidadPlanificable){
	// segun algoritmo calcular numero
	return 0;
}

void destruirPlanificador(Planificador * this, void (*destructorUnidadPlanificable)(UnidadPlanificable *)) {
    log_destroy(this->logger);
    this->transicionadorDeEstados.destruir(&this->transicionadorDeEstados);
    destruirColasDePlanificacion(this->colas, destructorUnidadPlanificable);
}

static Planificador new() { // TODO: asignar servicioDeMetricas que tiene que llegar por parametro.
    t_log * logger = log_create(TEAM_INTERNAL_LOG_FILE, "Planificador", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);

    char * nombreAlgoritmo = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, ALGORITMO_PLANIFICACION);
    log_info(logger, "El planificador se inicializará con el algoritmo %s", nombreAlgoritmoCompleto(nombreAlgoritmo));

    Planificador planificador = {
            .logger = logger,
            .algoritmoPlanificador = obtenerAlgoritmo(nombreAlgoritmo),
            .transicionadorDeEstados = TransicionadorDeEstadosConstructor.new(),
            .colas = crearColasDePlanificacion(),
            &agregarUnidadesPlanificables,
            &agregarUnidadPlanificable,

			&armarListaEntrenadoresDisponibles,
			&obtenerProximoAEjecutar,
			&cantidadDeRafagas,

            &destruirPlanificador,
    };

    return planificador;
}

const struct PlanificadorClass PlanificadorConstructor = {.new=&new};
