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

t_list* armarListaEntrenadoresDisponibles(Planificador * this) {
	// Retorna los de New, Blocked sin Espera y no llenos.
	t_list* entrenadoresDisponibles = list_duplicate(this->colas->colaNew); // Si hago duplicate a lista vacia, me retorna lista vacia?
	t_list* auxiliarBlocked = list_duplicate(this->colas->colaBlocked);
	for (int a = 0; a < list_size(auxiliarBlocked); a++) {
		HiloEntrenadorPlanificable* elementoIterado = (HiloEntrenadorPlanificable*)list_get(auxiliarBlocked,a);
		if (!elementoIterado->entrenador->estaEsperandoAlgo && elementoIterado->entrenador->puedeAtraparPokemones(elementoIterado->entrenador)){
			list_add(entrenadoresDisponibles, elementoIterado);
		}
	}
	list_destroy(auxiliarBlocked);
	return entrenadoresDisponibles;
}

UnidadPlanificable* obtenerProximoAEjecutar(Planificador * this) {
	return this->algoritmoPlanificador.proximoAEjecutar(&this->algoritmoPlanificador, this->colas->colaReady);
}

int cantidadDeRafagas(Planificador * planificador, UnidadPlanificable * unidadPlanificable){
	// segun algoritmo calcular numero
	//planificador->algoritmoPlanificador->tipo;
	return 0;
}

void destruirPlanificador(Planificador * this, void (*destructorUnidadPlanificable)(UnidadPlanificable *)) {
    log_destroy(this->logger);
    this->transicionadorDeEstados.destruir(&this->transicionadorDeEstados);
    destruirColasDePlanificacion(this->colas, destructorUnidadPlanificable);
}

static Planificador new(ServicioDeMetricas* servicio) { // TODO: asignar servicioDeMetricas que tiene que llegar por parametro.
    t_log * logger = log_create(TEAM_INTERNAL_LOG_FILE, "Planificador", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);

    char * nombreAlgoritmo = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, ALGORITMO_PLANIFICACION);
    log_info(logger, "El planificador se inicializará con el algoritmo %s", nombreAlgoritmoCompleto(nombreAlgoritmo));

    Planificador planificador = {
            .logger = logger,
            .algoritmoPlanificador = obtenerAlgoritmo(nombreAlgoritmo),
            .transicionadorDeEstados = TransicionadorDeEstadosConstructor.new(),
            .colas = crearColasDePlanificacion(),
			.servicioDeMetricas = servicio,
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
