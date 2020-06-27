//
// Created by Alan Zhao on 13/06/2020.
//

#include "planificador/algoritmos/fifo/FirstInFirstOut.h"

static UnidadPlanificable * proximoAEjecutar(AlgoritmoPlanificador * this, t_list * listaReady) {
    if (list_is_empty(listaReady)) {
        log_error(this->logger, "No hay nada para planificar.");
        return NULL;
    } else {
        UnidadPlanificable * unindadProximaAEjecutar = list_get(listaReady, 0);
        log_debug(this->logger, "Proxima unidad planificable a ejecutar seleccionada con éxito");
        return unindadProximaAEjecutar;
    }
}

static void destruir(AlgoritmoPlanificador * this) {
    log_destroy(this->logger);
}

static AlgoritmoPlanificador new() {
    return (AlgoritmoPlanificador) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "AlgoritmoFifo", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
            .tipo = FIFO,
            &proximoAEjecutar,
            &destruir
    };
}

const struct FirstInFirstOutClass FirstInFirstOutConstructor = {.new=&new};