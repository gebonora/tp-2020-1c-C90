//
// Created by Alan Zhao on 11/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDePlanificador() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestPlanificador", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando al planificador");

    Entrenador * entrenador1 = EntrenadorConstructor.new("0|0", "A", "B");
    Entrenador * entrenador2 = EntrenadorConstructor.new("0|0", "A", "B");
    Entrenador * entrenador3 = EntrenadorConstructor.new("0|0", "A", "B");
    HiloEntrenadorPlanificable * entrenadorPlanificable1 = HiloEntrenadorPlanificableConstructor.new(entrenador1);
    HiloEntrenadorPlanificable * entrenadorPlanificable2 = HiloEntrenadorPlanificableConstructor.new(entrenador2);
    HiloEntrenadorPlanificable * entrenadorPlanificable3 = HiloEntrenadorPlanificableConstructor.new(entrenador3);
    ServicioDeMetricas* metricasTest = ServicioDeMetricasConstructor.new();
    Planificador planificador = PlanificadorConstructor.new(metricasTest);

    //TODO: Test de planificador
    log_info(testLogger, "Testeando que una nueva unidad planificable vayan a parar a NEW");
    planificador.agregarUnidadPlanificable(&planificador, entrenadorPlanificable1);
    assert(list_size(planificador.colas->colaNew) == 1);

    t_list * planificables = list_create();
    list_add(planificables, entrenadorPlanificable2);
    list_add(planificables, entrenadorPlanificable3);

    log_info(testLogger, "Testeando que varias nuevas unidades planificables vayan a parar a NEW");
    planificador.agregarUnidadesPlanificables(&planificador, planificables);
    assert(list_size(planificador.colas->colaNew) == 3);

    planificador.destruir(&planificador, destruirUnidadPlanificable);

    entrenador1->destruir(entrenador1);
    entrenador2->destruir(entrenador2);
    entrenador3->destruir(entrenador3);
    list_destroy(planificables);
    log_destroy(testLogger);
    metricasTest->destruir(metricasTest);
}
