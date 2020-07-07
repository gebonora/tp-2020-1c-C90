//
// Created by Alan Zhao on 07/07/2020.
//

#include "modelo/planificable/tarea/TareaPlanificable.h"

static TareaPlanificable *new() {
    TareaPlanificable * tarea = malloc(sizeof(TareaPlanificable));

    tarea->logger = log_create(TEAM_INTERNAL_LOG_FILE, "TareaPlanificable", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);

    return tarea;
}

const struct TareaPlanificableClass TareaPlanificableConstructor = {.new=&new};