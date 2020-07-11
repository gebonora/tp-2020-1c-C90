//
// Created by Alan Zhao on 11/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDePlanificador() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestPlanificador", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando al planificador");

    Entrenador * entrenador1 = EntrenadorConstructor.new("0|0", "A", "B");
    HiloEntrenadorPlanificable * entrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenador1);
    Planificador planificador = PlanificadorConstructor.new();
    planificador.agregarUnidadPlanificable(&planificador, entrenadorPlanificable);

    //TODO: Test de planificador

    planificador.destruir(&planificador, destruirUnidadPlanificable);

    log_destroy(testLogger);
}