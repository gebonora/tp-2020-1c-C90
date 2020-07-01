//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/planificable/HiloEntrenadorPlanificable.h"
#include <unistd.h>

static void trabajar(HiloEntrenadorPlanificable * this) {
    while (true) {
        log_debug(this->logger, "Trabajo muy duro");
        sleep(3);
    }
}

static void destruir(HiloEntrenadorPlanificable * this) {
    log_destroy(this->logger);
    pthread_mutex_destroy(&this->mutex);
    free(this);
}

static HiloEntrenadorPlanificable *new(Entrenador * entrenador) {
    HiloEntrenadorPlanificable *hiloEntrenadorPlanificable = malloc(sizeof(HiloEntrenadorPlanificable));

    char * nombreLog = string_from_format("HiloEntrenadorPlanificable-%s", entrenador->id);
    hiloEntrenadorPlanificable->logger = log_create(TEAM_INTERNAL_LOG_FILE, nombreLog, SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
    hiloEntrenadorPlanificable->entrenador = entrenador;
    pthread_mutex_init(&hiloEntrenadorPlanificable->mutex, NULL);
    hiloEntrenadorPlanificable->trabajar = &trabajar;
    hiloEntrenadorPlanificable->destruir = &destruir;

    free(nombreLog);

    return hiloEntrenadorPlanificable;
}

const struct HiloEntrenadorPlanificableClass HiloEntrenadorPlanificableConstructor = {.new=&new};