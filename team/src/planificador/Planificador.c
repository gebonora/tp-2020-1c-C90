//
// Created by Alan Zhao on 11/07/2020.
//

#include "planificador/Planificador.h"

void agregarUnidadesPlanificables(Planificador * this, t_list * unidadesPlanificables) {
    list_add_all(this->colas->colaNew, unidadesPlanificables);
}

void agregarUnidadPlanificable(Planificador * this, UnidadPlanificable * unidadPlanificable) {
    list_add(this->colas->colaNew, unidadPlanificable);
}

void destruirPlanificador(Planificador * this) {
    log_destroy(this->logger);
    this->transicionadorDeEstados.destruir(&this->transicionadorDeEstados);
    destruirColasDePlanificacion(this->colas, this->destructorUnidadPlanificable);
}

static Planificador new(t_list * unidadesPlanificablesIniciales, void (*destructorUnidadPlanificable)(UnidadPlanificable *)) {
    t_log * logger = log_create(TEAM_INTERNAL_LOG_FILE, "Planificador", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);

    char * nombreAlgoritmo = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, ALGORITMO_PLANIFICACION);
    log_info(logger, "El planificador se inicializará con el algoritmo %s", nombreAlgoritmoCompleto(nombreAlgoritmo));

    Planificador planificador = {
            .logger = logger,
            .algoritmoPlanificador = obtenerAlgoritmo(nombreAlgoritmo),
            .transicionadorDeEstados = TransicionadorDeEstadosConstructor.new(),
            .colas = crearColasDePlanificacion(),
            &agregarUnidadesPlanificables,
            &agregarUnidadPlanificable,
            &destruirPlanificador,
            destructorUnidadPlanificable,
    };

    if (unidadesPlanificablesIniciales != NULL) {
        planificador.agregarUnidadesPlanificables(&planificador, unidadesPlanificablesIniciales);
    }

    return planificador;
}

const struct PlanificadorClass PlanificadorConstructor = {.new=&new};