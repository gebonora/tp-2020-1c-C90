#include "planificador/algoritmos/rr/RoundRobin.h"

static UnidadPlanificable * proximoAEjecutar(AlgoritmoPlanificador * this, t_list * listaReady) {
    if (list_is_empty(listaReady)) {
        log_error(this->logger, "No hay nada para planificar.");
        return NULL;
    } else {
        UnidadPlanificable * unindadProximaAEjecutar = list_get(listaReady, 0);
        log_debug(this->logger, "Proxima unidad planificable a ejecutar seleccionada con éxito");
        return unindadProximaAEjecutar;
    }
}// es igual que en fifo ya que los saca de la lista con el mismo criterio



static void destruir(AlgoritmoPlanificador * this) {
    log_destroy(this->logger);
}

static AlgoritmoPlanificador new() {
    return (AlgoritmoPlanificador) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "AlgoritmoRR", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
            .tipo = RR,
            &proximoAEjecutar,
            &destruir
    };
}

const struct RoundRobinClass RoundRobinConstructor = {.new=&new};
