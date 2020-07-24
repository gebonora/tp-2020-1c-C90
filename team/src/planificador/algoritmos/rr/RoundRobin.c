#include "planificador/algoritmos/rr/RoundRobin.h"

//TODO: Falta definir el lugar donde se le dira al proceso que se ejecute parcialmente

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

void obtenerConfiguracionesRR() {
	t_config* configRR = config_create(TEAM_CONFIG_FILE);

	quantum = config_get_int_value(configRR, "QUANTUM");

	config_destroy(configRR);
}

static void destruir(AlgoritmoPlanificador * this) {
    log_destroy(this->logger);
}

static AlgoritmoPlanificador new() {
	obtenerConfiguracionesRR();
    return (AlgoritmoPlanificador) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "AlgoritmoRR", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
            .tipo = RR,
            &proximoAEjecutar,
            &destruir
    };
}

const struct RoundRobinClass RoundRobinConstructor = {.new=&new};
