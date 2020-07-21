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

	ServicioDeMetricas* servicioMetricasTest = ServicioDeMetricasConstructor.new();
	ServicioDeResolucionDeDeadlocks* servicioDeadlockTest = ServicioDeResolucionDeDeadlocksConstructor.new(servicioMetricasTest);

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

	t_list* resultado = servicioDeadlockTest->procesarDeadlock(servicioDeadlockTest, listaEntrenadores);
	list_destroy_and_destroy_elements(resultado, destruirIntercambio);

	log_info(testLogger, "Deberia haber dos deadlocks: ASH y GANDALF. BROCK y ARAGORN");
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

	servicioDeadlockTest->primeraVez = true;
	t_list* resultado1 = servicioDeadlockTest->procesarDeadlock(servicioDeadlockTest, lista2);
	list_destroy_and_destroy_elements(resultado1, destruirIntercambio);

	log_info(testLogger, "Deberia haber tres deadlocks: ");

	t_list* lista3 = list_create();

	Entrenador * e1 = EntrenadorConstructor.new("2|4", "A", "B");
	free(e1->id);
	e1->id = string_from_format("D1E1");

	Entrenador * e2 = EntrenadorConstructor.new("2|4", "B", "C");
	free(e2->id);
	e2->id = string_from_format("D1E2");

	Entrenador * e3 = EntrenadorConstructor.new("2|4", "C", "A");
	free(e3->id);
	e3->id = string_from_format("D1E3");

	Entrenador * e4 = EntrenadorConstructor.new("2|4", "1", "2");
	free(e4->id);
	e4->id = string_from_format("circularE1");

	Entrenador * e5 = EntrenadorConstructor.new("2|4", "2", "3");
	free(e5->id);
	e5->id = string_from_format("ciruclarE2");

	Entrenador * e6 = EntrenadorConstructor.new("2|4", "3", "4");
	free(e6->id);
	e6->id = string_from_format("circukarE3");

	Entrenador * e7 = EntrenadorConstructor.new("2|4", "4", "1");
	free(e7->id);
	e7->id = string_from_format("circularE4");

	Entrenador * e8 = EntrenadorConstructor.new("2|4", "AA|CC|BB|ESTE", "AA|CC|BB|OTRO");
	free(e8->id);
	e8->id = string_from_format("D3E1");

	Entrenador * e9 = EntrenadorConstructor.new("2|4", "C|OTRO", "C|ESTE");
	free(e9->id);
	e9->id = string_from_format("D3E2");

	list_add(lista3, e1);
	list_add(lista3, e2);
	list_add(lista3, e3);
	list_add(lista3, e4);
	list_add(lista3, e5);
	list_add(lista3, e6);
	list_add(lista3, e7);
	list_add(lista3, e8);
	list_add(lista3, e9);

	servicioDeadlockTest->primeraVez = true;
	t_list* resultado2 = servicioDeadlockTest->procesarDeadlock(servicioDeadlockTest, lista3);
	list_destroy_and_destroy_elements(resultado2, destruirIntercambio);

	log_info(testLogger, "Debería haber deadlock entre todos:");

	t_list* lista4 = list_create();

	Entrenador * a1 = EntrenadorConstructor.new("2|4", "1|2", "6|5");
	free(a1->id);
	a1->id = string_from_format("a1");

	Entrenador * a2 = EntrenadorConstructor.new("2|4", "5", "6");
	free(a2->id);
	a2->id = string_from_format("a2");

	Entrenador * a3 = EntrenadorConstructor.new("2|4", "1|2|3", "4|5|2");
	free(a3->id);
	a3->id = string_from_format("a3");

	Entrenador * a4 = EntrenadorConstructor.new("2|4", "4|3", "1|2");
	free(a4->id);
	a4->id = string_from_format("a4");

	Entrenador * a5 = EntrenadorConstructor.new("2|4", "5|2", "3|1");
	free(a5->id);
	a5->id = string_from_format("a5");

	Entrenador * a6 = EntrenadorConstructor.new("2|4", "6|6", "2|3");
	free(a6->id);
	a6->id = string_from_format("a6");

	list_add(lista4, a1);
	list_add(lista4, a2);
	list_add(lista4, a3);
	list_add(lista4, a4);
	list_add(lista4, a5);
	list_add(lista4, a6);

	servicioDeadlockTest->primeraVez = true;
	t_list* resultado4 = servicioDeadlockTest->procesarDeadlock(servicioDeadlockTest, lista4);
	list_destroy_and_destroy_elements(resultado4, destruirIntercambio);

	servicioDeadlockTest->destruir(servicioDeadlockTest);
	servicioMetricasTest->destruir(servicioMetricasTest);
	log_destroy(testLogger);
	void killEntrenador(void* elem) {
		Entrenador* this = (Entrenador*) elem;
		this->destruir(this);
	}
	list_destroy_and_destroy_elements(listaEntrenadores, killEntrenador);
	list_destroy_and_destroy_elements(lista2, killEntrenador);
	list_destroy_and_destroy_elements(lista3, killEntrenador);
	list_destroy_and_destroy_elements(lista4, killEntrenador);
}
