//
// Created by Alan Zhao on 11/07/2020.
//

#include "planificador/transicionador/TransicionadorDeEstados.h"

ResultadoTransicion transicionarA(TransicionadorDeEstados * this, UnidadPlanificable * unidadPlanificable, EstadoPlanificador estadoDeseado) {
    return TRANSICION_EXITOSA;
}

void destruirTransicionadorDeEstados(TransicionadorDeEstados * this) {
    log_destroy(this->logger);
}

static TransicionadorDeEstados new() {
    return (TransicionadorDeEstados) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "TransicionadorDeEstados", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO),
            &transicionarA,
            &destruirTransicionadorDeEstados
    };
}

const struct TransicionadorDeEstadosClass TransicionadorDeEstadosConstructor = {.new=&new};