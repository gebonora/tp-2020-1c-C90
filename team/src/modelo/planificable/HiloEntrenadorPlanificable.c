//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/planificable/HiloEntrenadorPlanificable.h"

static void trabajar(HiloEntrenadorPlanificable * this) {
    while (!this->finDeTrabajo) {
        log_debug(this->logger, "Comienza un ciclo de trabajo muy duro");
        sem_wait(&this->semaforoEjecucionHabilitada);
        if (this->finDeTrabajo) {
            log_debug(this->logger, "Se interrumpió el ciclo de trabajo fin de trabajo");
            break;
        }
        log_debug(this->logger, "Finaliza un ciclo de trabajo muy duro");
    }
    sem_post(&this->semaforoFinDeTrabajo);
}

static void destruir(HiloEntrenadorPlanificable * this) {
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
    hiloEntrenadorPlanificable->finDeTrabajo = false;
    sem_init(&hiloEntrenadorPlanificable->semaforoEjecucionHabilitada,1 ,0);
    sem_init(&hiloEntrenadorPlanificable->semaforoFinDeTrabajo,0 ,0);
    hiloEntrenadorPlanificable->trabajar = &trabajar;
    hiloEntrenadorPlanificable->destruir = &destruir;
    free(nombreLog);

    crearHilo(hiloEntrenadorPlanificable->trabajar, hiloEntrenadorPlanificable);

    return hiloEntrenadorPlanificable;
}

const struct HiloEntrenadorPlanificableClass HiloEntrenadorPlanificableConstructor = {.new=&new};