//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/entrenador/Entrenador.h"

static void destruir(Entrenador * this) {
    log_destroy(this->logger);
    free(this);
}

static Entrenador *new() {
    Entrenador * entrenador = malloc(sizeof(Entrenador));

    entrenador->logger = log_create(TEAM_INTERNAL_LOG_FILE, "Entrenador", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    entrenador->destruir = &destruir;

    return entrenador;
}

const struct EntrenadorClass EntrenadorConstructor = {.new=&new};

Equipo crearEquipoPorConfiguracion() {
    // TODO: Uff lo que falta
    return list_create();
}
