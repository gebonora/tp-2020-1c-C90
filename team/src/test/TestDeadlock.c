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

	log_info(testLogger, "Espera Circular entre 3 entrenadores. Debería informar 1 dealocks con los 3");
	Entrenador * entrenador = EntrenadorConstructor.new("1|2", "X|Y|Z", "F|G|T"); // posicion, iniciales, objetivo.
	free(entrenador->id);
	entrenador->id = string_from_format("JUAN");

	Entrenador * entrenador2 = EntrenadorConstructor.new("2|4", "A|B|C", "X|Y|Z");
	free(entrenador2->id);
	entrenador2->id = string_from_format("PEPE");

	Entrenador * entrenador3 = EntrenadorConstructor.new("2|4", "F|G|T", "A|B|C");
	free(entrenador3->id);
	entrenador3->id = string_from_format("SHAQUILLE");

	list_add(listaEntrenadores, entrenador);
	list_add(listaEntrenadores, entrenador2);
	list_add(listaEntrenadores, entrenador3);

	servicioDeadlockTest->detectarDeadlock(servicioDeadlockTest, listaEntrenadores);

	log_info(testLogger, "Dos deadlocks: ASH y GANDALF. BROCK y ARAGORN");
	t_list* lista2 = list_create();
	Entrenador * entrenador4 = EntrenadorConstructor.new("1|2", "X|Y|A", "X|Y|G"); // posicion, iniciales, objetivo.
	free(entrenador4->id);
	entrenador4->id = string_from_format("ASH");

	Entrenador * entrenador5 = EntrenadorConstructor.new("2|4", "C", "B");
	free(entrenador5->id);
	entrenador5->id = string_from_format("BROCK");

	Entrenador * entrenador6 = EntrenadorConstructor.new("2|4", "B", "C");
	free(entrenador6->id);
	entrenador6->id = string_from_format("ARAGORN");

	Entrenador * entrenador7 = EntrenadorConstructor.new("2|4", "H|G", "A|H");
	free(entrenador7->id);
	entrenador7->id = string_duplicate("GANDALF");



	list_add(lista2, entrenador4);
	list_add(lista2, entrenador5);
	list_add(lista2, entrenador6);
	list_add(lista2, entrenador7);

	servicioDeadlockTest->detectarDeadlock(servicioDeadlockTest, lista2);

	entrenador->destruir(entrenador);
	entrenador2->destruir(entrenador2);
	entrenador3->destruir(entrenador3);
	entrenador4->destruir(entrenador4);
	entrenador5->destruir(entrenador5);
	entrenador6->destruir(entrenador6);
	entrenador7->destruir(entrenador7);

	servicioDeadlockTest->destruir(servicioDeadlockTest);
	log_destroy(testLogger);
	list_destroy(listaEntrenadores);
	list_destroy(lista2);

}
