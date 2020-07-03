//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/planificable/HiloEntrenadorPlanificable.h"

static void trabajar(HiloEntrenadorPlanificable * this) {
    while (!this->finDeTrabajo) {
        //TODO: Ver como PAUSAR el hilo y que se pueda liberar aunque este pausado.
        log_debug(this->logger, "Trabajo muy duro");
    }
    log_debug(this->logger, "Fin del hilo de trabajo");
    sem_post(&this->semaforoFinDeTrabajo);
}

static void destruir(HiloEntrenadorPlanificable * this) {
    log_debug(this->logger, "Se procede a destruir al hilo de ejecucion del entrenador");
    this->finDeTrabajo = true;
    sem_wait(&this->semaforoFinDeTrabajo);
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
    hiloEntrenadorPlanificable->finDeTrabajo = true;
    sem_init(&hiloEntrenadorPlanificable->semaforoEjecucionHabilitada,1 ,0);
    sem_init(&hiloEntrenadorPlanificable->semaforoFinDeTrabajo,0 ,0);
    hiloEntrenadorPlanificable->trabajar = &trabajar;
    hiloEntrenadorPlanificable->destruir = &destruir;
    free(nombreLog);

    crearHilo(hiloEntrenadorPlanificable->trabajar, hiloEntrenadorPlanificable);

    return hiloEntrenadorPlanificable;
}

const struct HiloEntrenadorPlanificableClass HiloEntrenadorPlanificableConstructor = {.new=&new};