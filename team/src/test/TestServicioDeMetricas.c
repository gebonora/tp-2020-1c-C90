/*
 * TestServicioDeMetricas.c
 *
 *  Created on: 17 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "test/TestDeIntegracion.h"

void testDeServicioDeMetricas() {
	t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestDeServicioDeMetricas", 1, LOG_LEVEL_INFO);
	log_info(testLogger, "Testeando el servicio de métricas");

	ServicioDeMetricas* servicioDeMetricasTest = ServicioDeMetricasConstructor.new();

	servicioDeMetricasTest->registrarCambioDeContexto(servicioDeMetricasTest);
	servicioDeMetricasTest->registrarCambioDeContexto(servicioDeMetricasTest);
	servicioDeMetricasTest->registrarDeadlockProducido(servicioDeMetricasTest);
	servicioDeMetricasTest->registrarDeadlockResuelto(servicioDeMetricasTest);
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "GONZA");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "ASHKETCHUP");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "ASHKETCHUP");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "MISTY");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "ASHKETCHUP");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "BROCK");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "BROCK");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "ASHKETCHUP");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "ASHKETCHUP");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "ASHKETCHUP");
	servicioDeMetricasTest->registrarCicloRealizadoPorEntrenador(servicioDeMetricasTest, "GONZA");

	servicioDeMetricasTest->imprimirMetricas(servicioDeMetricasTest);

	assert(servicioDeMetricasTest->cantidadCambiosDeContexto == 2);
	assert(servicioDeMetricasTest->cantidadDeadlocksProducidos == 1);
	assert(servicioDeMetricasTest->calcularCiclosTotales(servicioDeMetricasTest) == 11);
	assert((int) dictionary_get(servicioDeMetricasTest->hashMap, "GONZA") == 2);
	assert((int) dictionary_get(servicioDeMetricasTest->hashMap, "BROCK") == 2);
	assert((int) dictionary_get(servicioDeMetricasTest->hashMap, "ASHKETCHUP") == 6);
	assert((int) dictionary_get(servicioDeMetricasTest->hashMap, "MISTY") == 1);

	servicioDeMetricasTest->destruir(servicioDeMetricasTest);
	log_destroy(testLogger);
}

