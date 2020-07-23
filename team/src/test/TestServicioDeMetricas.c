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

	t_list* listaDeListas = list_create();
	t_list* lista = list_create();
	list_add(lista, string_duplicate("OBIWAN"));
	list_add(lista, string_duplicate("ANAKIN"));
	list_add(lista, string_duplicate("YODA"));
	list_add(lista, string_duplicate("WINDU"));

	t_list* lista2 = list_create();
	list_add(lista2, string_duplicate("A"));
	list_add(lista2, string_duplicate("B"));
	list_add(lista2, string_duplicate("C"));
	list_add(lista2, string_duplicate("D"));

	list_add(listaDeListas, lista);
	list_add(listaDeListas, lista2);

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
	servicioDeMetricasTest->registrarDeadlocks(servicioDeMetricasTest, listaDeListas);
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "A");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "WINDU");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "B");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "B");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "B");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "D");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "C");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "YODA");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "ANAKIN");
	servicioDeMetricasTest->registrarIntercambio(servicioDeMetricasTest, "HARAMBE");

	servicioDeMetricasTest->imprimirMetricas(servicioDeMetricasTest);

	assert(servicioDeMetricasTest->cantidadCambiosDeContexto == 2);
	assert(servicioDeMetricasTest->cantidadDeadlocksProducidos == 1);
	assert(servicioDeMetricasTest->calcularCiclosTotales(servicioDeMetricasTest) == 11);
	assert((int ) dictionary_get(servicioDeMetricasTest->hashMap, "GONZA") == 2);
	assert((int ) dictionary_get(servicioDeMetricasTest->hashMap, "BROCK") == 2);
	assert((int ) dictionary_get(servicioDeMetricasTest->hashMap, "ASHKETCHUP") == 6);
	assert((int ) dictionary_get(servicioDeMetricasTest->hashMap, "MISTY") == 1);
	assert((int) (((RegistroDeadlock*) list_get(servicioDeMetricasTest->deadlocks, 0))->cantidadIntercambios) == 3);
	assert((int) (((RegistroDeadlock*) list_get(servicioDeMetricasTest->deadlocks, 1))->cantidadIntercambios) == 6);

	servicioDeMetricasTest->destruir(servicioDeMetricasTest);
	log_destroy(testLogger);
	list_destroy_and_destroy_elements(lista, free);
	list_destroy_and_destroy_elements(lista2, free);
	list_destroy(listaDeListas);
}

