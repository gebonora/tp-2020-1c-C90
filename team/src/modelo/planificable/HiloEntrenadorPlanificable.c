//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/planificable/HiloEntrenadorPlanificable.h"

static void trabajar(HiloEntrenadorPlanificable * this) {
	while (!this->finDeTrabajo) {
		log_debug(this->logger, "Comienza un ciclo de trabajo muy duro");

		sem_wait(&this->semaforoEjecucionHabilitada);

		if (this->finDeTrabajo) {
			log_debug(this->logger, "Se interrumpió el ciclo de trabajo por fin de trabajo");
			break;
		}

		TareaPlanificable * tareaEnEjecucion = this->tareaAsignada;
		Instruccion * instruccion = tareaEnEjecucion->proximaInstruccion(tareaEnEjecucion);
		log_info(this->logger, "Se procede a ejecutar la instruccion %d: %s.", instruccion->posicion, instruccion->descripcion);

		ejecutarInstruccion(instruccion, this->entrenador);

		if (this->retardoInstruccion > 0) {
			for (int r = this->retardoInstruccion; r > 0; r--) {
				log_debug(this->logger, "Fin de retardo en %d...", r);
				sleep(1);
			}
		}

		tareaEnEjecucion->notificarEjecucion(tareaEnEjecucion, instruccion->posicion);

		servicioDeMetricasProcesoTeam->registrarCicloRealizadoPorEntrenador(servicioDeMetricasProcesoTeam, this->entrenador->id);

		log_debug(this->logger, "Finaliza un ciclo de trabajo muy duro");

		sem_post(&this->semaforoCicloCompletado);
	}
	sem_post(&this->semaforoFinDeTrabajo);
}

static void asignarTarea(HiloEntrenadorPlanificable * this, TareaPlanificable * tarea) {
	if (tarea->estado == PENDIENTE_DE_EJECUCION) {
		this->tareaAsignada = tarea;
	} else {
		log_error(this->logger, "No se puede asignar una tarea con estado %s", nombreEstadoTareaPlanificable(tarea->estado));
	}
}

static void ejecutarLimitado(HiloEntrenadorPlanificable * this, int cantInstrucciones) {
	TareaPlanificable * tarea = this->tareaAsignada;
	if (tarea->estado != PENDIENTE_DE_EJECUCION) {
		log_error(this->logger, "Se esta intentando ejecutar una tarea con estado %s", nombreEstadoTareaPlanificable(tarea->estado));
		return;
	}

    log_debug(this->logger, "Ejecutando tarea. Instrucciones a correr: %d/%d restantes. Total: %d.",
              cantInstrucciones, tarea->cantidadInstruccionesRestantes(tarea), tarea->totalInstrucciones);
	for (int i = 0; i < cantInstrucciones; i++) {
		sem_post(&this->semaforoEjecucionHabilitada);
		sem_wait(&this->semaforoCicloCompletado);
	}
	// this->infoUltimaEjecucion.real_raf_ant = cantInstrucciones;
	this->infoUltimaEjecucion.rafaga_parcial_ejecutada = tarea->cantidadInstruccionesEjecutadas(tarea); // TODO: chequear si hace falta algo mas con Gaston.
	if (tarea->estado == FINALIZADA) {
		log_info(this->logger, "La tarea asignada se completó con exito. Se procede a destruirla.");
		tarea->destruir(tarea);
		this->tareaAsignada = NULL;
	}
}

static void ejecutar(HiloEntrenadorPlanificable * this) {
	this->ejecutarParcialmente(this, this->tareaAsignada->cantidadInstruccionesRestantes(this->tareaAsignada));
}

static void destruir(HiloEntrenadorPlanificable * this) {
	if (this->tareaAsignada != NULL) {
		log_warning(this->logger, "Todavia hay una tarea pendiente asignada. Se procede a abortarla y destruirla legalmente.");
		this->tareaAsignada->abortar(this->tareaAsignada);
		this->tareaAsignada->destruir(this->tareaAsignada);
	}
	this->finDeTrabajo = true;
	sem_post(&this->semaforoEjecucionHabilitada);
	sem_wait(&this->semaforoFinDeTrabajo);
	log_debug(this->logger, "Se procede a destruir la unidad planificable");
	log_destroy(this->logger);
	sem_destroy(&this->semaforoEjecucionHabilitada);
	sem_destroy(&this->semaforoFinDeTrabajo);
	free(this);
}

static HiloEntrenadorPlanificable *new(Entrenador * entrenador) {
	HiloEntrenadorPlanificable *hiloEntrenadorPlanificable = malloc(sizeof(HiloEntrenadorPlanificable));

	char * nombreLog = string_from_format("HiloEntrenadorPlanificable-%s", entrenador->id);
	hiloEntrenadorPlanificable->logger = log_create(TEAM_INTERNAL_LOG_FILE, nombreLog, SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
	hiloEntrenadorPlanificable->entrenador = entrenador;
	hiloEntrenadorPlanificable->retardoInstruccion = ACTIVAR_RETARDO_CPU ? servicioDeConfiguracion.obtenerEntero(&servicioDeConfiguracion, RETARDO_CICLO_CPU) : 0;
	hiloEntrenadorPlanificable->finDeTrabajo = false;
	sem_init(&hiloEntrenadorPlanificable->semaforoEjecucionHabilitada, 1, 0);
	sem_init(&hiloEntrenadorPlanificable->semaforoCicloCompletado, 1, 0);
	sem_init(&hiloEntrenadorPlanificable->semaforoFinDeTrabajo, 1, 0);
	hiloEntrenadorPlanificable->tareaAsignada = NULL;
	hiloEntrenadorPlanificable->infoUltimaEjecucion = (InfoUltimaEjecucion ) { .real_raf_ant = 0, .est_raf_ant = 0, .seEjecutoPrimeraEstimacion = false,
					.rafaga_parcial_ejecutada = 0, .rafaga_real_actual = 0, .seNecesitaNuevaEstimacion = true };
	hiloEntrenadorPlanificable->asignarTarea = &asignarTarea;
	hiloEntrenadorPlanificable->trabajar = &trabajar;
	hiloEntrenadorPlanificable->ejecutarParcialmente = &ejecutarLimitado;
	hiloEntrenadorPlanificable->ejecutar = &ejecutar;
	hiloEntrenadorPlanificable->destruir = &destruir;
	free(nombreLog);

	if (hiloEntrenadorPlanificable->retardoInstruccion <= 0) {
		log_warning(hiloEntrenadorPlanificable->logger, "No hay retardo artifical en los ciclos de CPU");
	}

	crearHilo((void *(*)(void *)) hiloEntrenadorPlanificable->trabajar, hiloEntrenadorPlanificable);

	return hiloEntrenadorPlanificable;
}

const struct HiloEntrenadorPlanificableClass HiloEntrenadorPlanificableConstructor = { .new = &new };
