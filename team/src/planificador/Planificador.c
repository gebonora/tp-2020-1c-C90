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
		HiloEntrenadorPlanificable* elementoIterado = (HiloEntrenadorPlanificable*) list_get(auxiliarBlocked, a);
		if (!elementoIterado->entrenador->estaEsperandoAlgo && elementoIterado->entrenador->puedeAtraparPokemones(elementoIterado->entrenador)) {
			list_add(entrenadoresDisponibles, elementoIterado);
		}
	}
	list_destroy(auxiliarBlocked);
	return entrenadoresDisponibles;

}

UnidadPlanificable* obtenerProximoAEjecutar(Planificador * this) {
	return this->algoritmoPlanificador.proximoAEjecutar(&this->algoritmoPlanificador, this->colas->colaReady);
}

int cantidadDeRafagas(Planificador * planificador, UnidadPlanificable * unidadPlanificable) {
	// segun algoritmo calcular numero
	int cantRafagas;

	switch (planificador->algoritmoPlanificador.tipo) {
	case FIFO: //FIFO
		cantRafagas = unidadPlanificable->tareaAsignada->totalInstrucciones;
		break;
	case RR: //RR
		;
		int quantum = servicioDeConfiguracion.obtenerEntero(&servicioDeConfiguracion, QUANTUM);
		cantRafagas = minimo(quantum, (unidadPlanificable->tareaAsignada->cantidadInstruccionesRestantes(unidadPlanificable->tareaAsignada)));
		break;
	case SJF_CD: //SJF-CD
		cantRafagas = 1;
		break;
	case SJF_SD: //SJF-SD
		cantRafagas = unidadPlanificable->tareaAsignada->totalInstrucciones;
		break;
	default:
		cantRafagas = 0;
		log_error(INTERNAL_LOGGER, "algoritmo planificador invalido");
	}
	return cantRafagas;
}

EstadoPlanificador obtenerEstadoDeUnidadPlanificable(Planificador* this, UnidadPlanificable* unidadPlanificable) {
	for (int a = 0; a < list_size(this->colas->colaExit); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(this->colas->colaExit, a);
		if (string_equals_ignore_case(unidadActual->entrenador->id, unidadPlanificable->entrenador->id)) {
			return EXIT;
		}
	}
	for (int a = 0; a < list_size(this->colas->colaBlocked); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(this->colas->colaBlocked, a);
		if (string_equals_ignore_case(unidadActual->entrenador->id, unidadPlanificable->entrenador->id)) {
			return BLOCK;
		}
	}
	for (int a = 0; a < list_size(this->colas->colaNew); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(this->colas->colaNew, a);
		if (string_equals_ignore_case(unidadActual->entrenador->id, unidadPlanificable->entrenador->id)) {
			return NEW_;
		}
	}
	for (int a = 0; a < list_size(this->colas->colaReady); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(this->colas->colaReady, a);
		if (string_equals_ignore_case(unidadActual->entrenador->id, unidadPlanificable->entrenador->id)) {
			return READY;
		}
	}
	for (int a = 0; a < list_size(this->colas->colaExec); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(this->colas->colaExec, a);
		if (string_equals_ignore_case(unidadActual->entrenador->id, unidadPlanificable->entrenador->id)) {
			return EXEC;
		}
	}
	return -1;
}

void moverACola(Planificador * this, UnidadPlanificable * uPlanificable, EstadoPlanificador estado, char* motivoCambio) {
	t_list* colaDestino, colaOrigen;
	EstadoPlanificador estadoOrigen = this->obtenerEstadoDeUnidadPlanificable(this, uPlanificable);
	colaDestino = colaSegunEstado(this, estado);
	colaOrigen = colaSegunEstado(this, estadoOrigen );

	for (int a = 0; a < list_size(colaOrigen); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(colaOrigen, a);
		if (string_equals_ignore_case(unidadActual->entrenador->id, uPlanificable->entrenador->id)) {
			list_remove(colaOrigen, a);
			break;
		}
	}
	list_add(colaDestino,uPlanificable);

	log_info(MANDATORY_LOGGER, "Se movio al entrenador con ID %s de la cola: a la cola: %s. Motivo: %s", uPlanificable->entrenador->id, estado, motivoCambio);
}

t_list* colaSegunEstado(Planificador* this, EstadoPlanificador estado) {
	switch (estado) {
	case NEW_:
		return this->colas->colaNew;
		break;
	case BLOCK:
		return this->colas->colaBlocked;
		break;
	case READY:
		return this->colas->colaReady;
		break;
	case EXEC:
		return this->colas->colaExec;
		;
		break;
	case EXIT:
		return this->colas->colaExit;
		break;
	}
	return NULL;
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

	Planificador planificador = { .logger = logger, .algoritmoPlanificador = obtenerAlgoritmo(nombreAlgoritmo), .transicionadorDeEstados =
			TransicionadorDeEstadosConstructor.new(), .colas = crearColasDePlanificacion(), .servicioDeMetricas = servicio, &agregarUnidadesPlanificables,
			&agregarUnidadPlanificable,

			&armarListaEntrenadoresDisponibles, &obtenerProximoAEjecutar, &cantidadDeRafagas,

			&moverACola, &obtenerEstadoDeUnidadPlanificable, &colaSegunEstado, &destruirPlanificador, };

	return planificador;
}

const struct PlanificadorClass PlanificadorConstructor = { .new = &new };

int minimo(int nro1, int nro2) {
	int min;
	if (nro1 <= nro2) {
		min = nro1;
	} else {
		min = nro2;
	}
	return min;
}

