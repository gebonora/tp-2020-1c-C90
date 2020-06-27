//
// Created by Alan Zhao on 10/06/2020.
//

#include "manejadorDeEventos/ManejadorDeEventos.h"

static void notificarEventoLocalized(ManejadorDeEventos * this) {
    log_debug(this->logger, "Llego un LOCALIZED");
}

static void destruir(ManejadorDeEventos * this) {
    log_destroy(this->logger);
}

static ManejadorDeEventos new() {
    return (ManejadorDeEventos) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "ManejadorDeEventos", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
            &notificarEventoLocalized,
            &destruir
    };
}

const struct ManejadorDeEventosClass ManejadorDeEventosConstructor = {.new=&new};