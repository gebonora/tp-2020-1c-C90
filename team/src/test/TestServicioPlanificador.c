//
// Created by Alan Zhao on 11/07/2020.
//

#include "test/TestDeIntegracion.h"

void testServicioDePlanificacion() {
	t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestServicioPlanificacion", 1, LOG_LEVEL_INFO);

	log_info(testLogger, "Testeando el servicio de planificacion");

	Equipo miEquipo = list_create();
	Entrenador * entrenador1 = EntrenadorConstructor.new("0|0", "A", "B");
	list_add(miEquipo, entrenador1);

	ServicioDeMetricas* metricasTest = ServicioDeMetricasConstructor.new();
	ServicioDeResolucionDeDeadlocks* deadlocksTest = ServicioDeResolucionDeDeadlocksConstructor.new(metricasTest);
	ServicioDePlanificacion * servicio = ServicioDePlanificacionConstructor.new(metricasTest, deadlocksTest);
	log_info(testLogger, "Testeando que el equipo a planificar quede en el planificador");
	servicio->asignarEquipoAPlanificar(servicio, miEquipo);

	assert(list_size(servicio->planificador.colas->colaNew) == 1);
	UnidadPlanificable * unidadPlanificable1 = list_get(servicio->planificador.colas->colaNew, 0);
	assert(string_equals(unidadPlanificable1->entrenador->id, entrenador1->id));

	//TODO: Tests de trabajos.

	servicio->destruir(servicio);
	metricasTest->destruir(metricasTest);
	deadlocksTest->destruir(deadlocksTest);
	destruirEquipo(miEquipo);
	log_destroy(testLogger);
}
