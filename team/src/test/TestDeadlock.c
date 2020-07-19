/*
 * TestDeadlock.c
 *
 *  Created on: 19 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "test/TestDeIntegracion.h"

void testDeadlock() {
	t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestDeadlock", 1, LOG_LEVEL_DEBUG);
	log_info(testLogger, "Testeando deadlocks");

	ServicioDeResolucionDeDeadlocks* servicioDeadlockTest = ServicioDeResolucionDeDeadlocksConstructor.new();

	t_list* listaEntrenadores = list_create();

	Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B|C|Z|X", "A|C|C|G|V"); // posicion, iniciales, objetivo.
	entrenador->id = "JUAN";
	Entrenador * entrenador2 = EntrenadorConstructor.new("2|4", "D|B", "E|B");
	entrenador2-> id = "PEPE";

	list_add(listaEntrenadores, entrenador);
	list_add(listaEntrenadores, entrenador2);

	servicioDeadlockTest->detectarDeadlock(servicioDeadlockTest, listaEntrenadores);

}
