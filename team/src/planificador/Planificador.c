//
// Created by Alan Zhao on 11/07/2020.
//

#include "planificador/Planificador.h"

void agregarUnidadesPlanificables(Planificador * this, t_list * unidadesPlanificables) {
	puts("ENTRE");
	printf("unidDFGFDSGSDFGDSFGades planif:%d \n\n", list_size(unidadesPlanificables));
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
	case FIFO:
		cantRafagas = unidadPlanificable->tareaAsignada->totalInstrucciones;
		break;
	case RR:
		cantRafagas = minimo(planificador->quantum, (unidadPlanificable->tareaAsignada->cantidadInstruccionesRestantes(unidadPlanificable->tareaAsignada)));
		break;
	case SJF_CD:
		cantRafagas = 1;
		break;
	case SJF_SD:
		cantRafagas = unidadPlanificable->tareaAsignada->totalInstrucciones;
		break;
	default:
		cantRafagas = 1;
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

t_list* colaSegunEstado(Planificador* this, EstadoPlanificador estado) {
	switch (estado) {
	case NEW_:
		return this->colas->colaNew;
	case BLOCK:
		return this->colas->colaBlocked;
	case READY:
		return this->colas->colaReady;
	case EXEC:
		return this->colas->colaExec;
	case EXIT:
		return this->colas->colaExit;
	}
	return NULL;
}

void moverACola(Planificador * this, UnidadPlanificable * uPlanificable, EstadoPlanificador estadoDestino, char* motivoCambio) {
	t_list* colaDestino;
	t_list* colaOrigen;
	EstadoPlanificador estadoOrigen = this->obtenerEstadoDeUnidadPlanificable(this, uPlanificable);
	colaDestino = colaSegunEstado(this, estadoDestino);
	colaOrigen = colaSegunEstado(this, estadoOrigen);

	for (int a = 0; a < list_size(colaOrigen); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(colaOrigen, a);
		if (string_equals_ignore_case(unidadActual->entrenador->id, uPlanificable->entrenador->id)) {
			list_remove(colaOrigen, a);
			break;
		}
	}
	list_add(colaDestino, uPlanificable);

	log_info(MANDATORY_LOGGER, "Se movio al entrenador %s de la cola:%s a la cola: %s. Motivo: %s", uPlanificable->entrenador->id, nombreDeLaCola(estadoOrigen),
			nombreDeLaCola(estadoDestino), motivoCambio);
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

	Planificador planificador = { .logger = logger, .quantum = servicioDeConfiguracion.obtenerEntero(&servicioDeConfiguracion, QUANTUM), .algoritmoPlanificador =
			obtenerAlgoritmo(nombreAlgoritmo), .transicionadorDeEstados = TransicionadorDeEstadosConstructor.new(), .colas = crearColasDePlanificacion(),
			.servicioDeMetricas = servicio, &agregarUnidadesPlanificables, &agregarUnidadPlanificable,

			&armarListaEntrenadoresDisponibles, &obtenerProximoAEjecutar, &cantidadDeRafagas,

			&colaSegunEstado, &moverACola, &obtenerEstadoDeUnidadPlanificable, &destruirPlanificador, };

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

char* nombreDeLaCola(EstadoPlanificador estado) {

	switch (estado) {
	case NEW_:
		return "NEW";
	case READY:
		return "READY";
	case EXEC:
		return "EXEC";
	case BLOCK:
		return "BLOCKED";
	case EXIT:
		return "EXIT";
	}
	return "ERROR";
}
