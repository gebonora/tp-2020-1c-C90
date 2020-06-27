//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/planificable/HiloEntrenadorPlanificable.h"

static void destruir(HiloEntrenadorPlanificable * this) {
    log_destroy(this->logger);
    free(this);
}

static HiloEntrenadorPlanificable *new(Entrenador * entrenador) {
    HiloEntrenadorPlanificable *hiloEntrenadorPlanificable = malloc(sizeof(HiloEntrenadorPlanificable));

    hiloEntrenadorPlanificable->entrenador = entrenador;
    hiloEntrenadorPlanificable->logger = log_create(TEAM_INTERNAL_LOG_FILE, "HiloEntrenadorPlanificable", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
    hiloEntrenadorPlanificable->destruir = &destruir;

    return hiloEntrenadorPlanificable;
}

const struct HiloEntrenadorPlanificableClass HiloEntrenadorPlanificableConstructor = {.new=&new};