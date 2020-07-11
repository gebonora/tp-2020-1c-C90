//
// Created by Alan Zhao on 11/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDePlanificador() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestPlanificador", 1, LOG_LEVEL_INFO);

    Planificador planificador = PlanificadorConstructor.new(NULL, destruirUnidadPlanificable);

    //TODO: Test de planificador

    planificador.destruir(&planificador);

    log_destroy(testLogger);
}