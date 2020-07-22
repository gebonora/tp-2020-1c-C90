//
// Created by Alan Zhao on 10/06/2020.
//

#include "test/TestDeIntegracion.h"

void testDeIntegracion() {
	t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestDeIntegracion", 1, LOG_LEVEL_INFO);
	t_list * tests = list_create();

	// Libs
	list_add(tests, testLibs);

	// Cliente Broker
	list_add(tests, testClienteBroker);

	// Entrenadores
	list_add(tests, testDeEntrenadores);

	// Pokemones
	list_add(tests, testDePokemones);

	// Movimiento
	list_add(tests, testDeMovimiento);

	// Mapa
	list_add(tests, testDeMapa);

	// Algoritmos
	list_add(tests, testDeAlgoritmos);

	// SJF sin Desalojo
	list_add(tests, testDeAlgoritmosSJFsinDesalojo);

	// Tareas
	list_add(tests, testDeTareas);

	// Planificador
	list_add(tests, testDePlanificador);

	// ServicioDePlanificacion
	list_add(tests, testServicioDePlanificacion);

	// ServicioDeCaptura
	list_add(tests, testServicioDeCaptura);

	// Unidad planificable
	list_add(tests, testDePlanificable);

	// Eventos
	list_add(tests, testDeEventos);

	// Servicio de Métricas
	list_add(tests, testDeServicioDeMetricas);

	// Servicio de Deadlock
	list_add(tests, testDeadlock);

	for (int i = 0; i < list_size(tests); i++) {
		log_info(testLogger, "-------- Ejecutando test %d/%d --------", i + 1, list_size(tests));
		((void (*)()) list_get(tests, i))();
	}

	list_destroy(tests);
	log_destroy(testLogger);
}
