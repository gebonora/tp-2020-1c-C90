//
// Created by Alan Zhao on 11/07/2020.
//

#include "planificador/Planificador.h"

void agregarUnidadesPlanificables(Planificador * this, t_list * unidadesPlanificables) {
	for (int a = 0; a < list_size(unidadesPlanificables); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(unidadesPlanificables, a);
		this->agregarUnidadPlanificable(this, unidadActual);
	}
}

void agregarUnidadPlanificable(Planificador * this, UnidadPlanificable * unidadPlanificable) {
	log_info(MANDATORY_LOGGER, "Se movió al entrenador %s a la cola: NEW. Motivo: Se generó un hilo de entrenador", unidadPlanificable->entrenador->id);
	list_add(this->colas->colaNew, unidadPlanificable);
	sem_post(&semaforoContadorEntrenadoresDisponibles);
}

t_list* armarListaEntrenadoresDisponibles(Planificador * this) {
	// Retorna los de New, Blocked sin Espera y no llenos.
	pthread_mutex_lock(&arrayMutexColas[NEW]);
	t_list* entrenadoresDisponibles = list_duplicate(this->colas->colaNew); // Si hago duplicate a lista vacia, me retorna lista vacia?
	pthread_mutex_unlock(&arrayMutexColas[NEW]);

	pthread_mutex_lock(&arrayMutexColas[BLOCK]);
	t_list* auxiliarBlocked = list_duplicate(this->colas->colaBlocked);
	pthread_mutex_unlock(&arrayMutexColas[BLOCK]);
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

	pthread_mutex_lock(&arrayMutexColas[estadoOrigen]);
	for (int a = 0; a < list_size(colaOrigen); a++) {
		UnidadPlanificable* unidadActual = (UnidadPlanificable*) list_get(colaOrigen, a);
		if (string_equals_ignore_case(unidadActual->entrenador->id, uPlanificable->entrenador->id)) {
			list_remove(colaOrigen, a);
			break;
		}
	}
	pthread_mutex_unlock(&arrayMutexColas[estadoOrigen]);

	pthread_mutex_lock(&arrayMutexColas[estadoDestino]);
	list_add(colaDestino, uPlanificable);
	pthread_mutex_unlock(&arrayMutexColas[estadoDestino]);

	log_info(MANDATORY_LOGGER, "Se movió al entrenador %s de la cola: %s a la cola: %s. Motivo: %s", uPlanificable->entrenador->id, nombreDeLaCola(estadoOrigen),
			nombreDeLaCola(estadoDestino), motivoCambio);
	switch (estadoDestino) {
	case READY:
		sem_post(&semaforoReady);
		break;
	case EXIT:
		sem_post(&semaforoObjetivoGlobalCompletado);
		sem_post(&semaforoDeadlock);
		break;
	case BLOCK:
		sem_post(&semaforoDeadlock);
		break;
	}
}

void mostrarLasColas(Planificador* this) {
	log_info(this->logger, "Cantidad en NEW: %d", list_size(this->colas->colaNew));
	log_info(this->logger, "Cantidad en READY: %d", list_size(this->colas->colaReady));
	log_info(this->logger, "Cantidad en EXEC: %d", list_size(this->colas->colaExec));
	log_info(this->logger, "Cantidad en BLOCKED: %d", list_size(this->colas->colaBlocked));
	log_info(this->logger, "Cantidad en EXIT: %d", list_size(this->colas->colaExit));
	int pokemon;
	int entrenadores;
	int ready;
	int deadlock;
	sem_getvalue(&semaforoContadorPokemon, &pokemon);
	sem_getvalue(&semaforoContadorEntrenadoresDisponibles, &entrenadores);
	sem_getvalue(&semaforoReady, &ready);
	sem_getvalue(&semaforoDeadlock, &deadlock);
	log_info(this->logger, "Semaforos. ContadorPokemon: %d, ContadorEntrenadoresDisponibles: %d, Ready: %d, Deadlock: %d", pokemon, entrenadores, ready, deadlock);

	t_list* mapa = mapaProcesoTeam.pokemonesDisponibles(&mapaProcesoTeam);
	log_info(this->logger, "Cantidad de Pokemon disponibles: %d", list_size(mapa));
	list_destroy_and_destroy_elements(mapa, free_pokemon);
}

HiloEntrenadorPlanificable* obtenerHiloSegunEntrenador(Planificador* this, Entrenador* entrenador) {
	bool trainerById(void* elem) {
		HiloEntrenadorPlanificable* hilo = elem;
		return hilo->entrenador->id = entrenador->id;
	}
	return list_find(this->colas->colaBlocked, trainerById);
}

void destruirPlanificador(Planificador * this, void (*destructorUnidadPlanificable)(UnidadPlanificable *)) {
	log_destroy(this->logger);
	this->transicionadorDeEstados.destruir(&this->transicionadorDeEstados);
	destruirColasDePlanificacion(this->colas, destructorUnidadPlanificable);
}

static Planificador new(ServicioDeMetricas* servicio) { // TODO: asignar servicioDeMetricas que tiene que llegar por parametro.
	t_log * logger = log_create(TEAM_INTERNAL_LOG_FILE, "Planificador", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
	char * nombreAlgoritmo = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, ALGORITMO_PLANIFICACION);
	log_info(logger, "El planificador se inicializará con el algoritmo %s", nombreAlgoritmoCompleto(nombreAlgoritmo));

	Planificador planificador = { .logger = logger, .quantum = servicioDeConfiguracion.obtenerEntero(&servicioDeConfiguracion, QUANTUM), .algoritmoPlanificador =
			obtenerAlgoritmo(nombreAlgoritmo), .transicionadorDeEstados = TransicionadorDeEstadosConstructor.new(), .colas = crearColasDePlanificacion(),
			.servicioDeMetricas = servicio, &agregarUnidadesPlanificables, &agregarUnidadPlanificable, &armarListaEntrenadoresDisponibles, &obtenerProximoAEjecutar,
			&cantidadDeRafagas, &colaSegunEstado, &moverACola, &obtenerEstadoDeUnidadPlanificable, &mostrarLasColas, &obtenerHiloSegunEntrenador, &destruirPlanificador, };
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
