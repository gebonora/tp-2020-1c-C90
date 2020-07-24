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

	log_info(testLogger, "Espera Circular entre 3 entrenadores. Debería informar 1 deadlock con los 3");
	Entrenador * _entrenador = EntrenadorConstructor.new("1|2", "X|Y|Z", "F|G|T"); // posicion, iniciales, objetivo.
	free(_entrenador->id);
	_entrenador->id = string_from_format("JUAN");
	HiloEntrenadorPlanificable* entrenador = HiloEntrenadorPlanificableConstructor.new(_entrenador);

	Entrenador * _entrenador2 = EntrenadorConstructor.new("2|4", "A|B|C", "X|Y|Z");
	free(_entrenador2->id);
	_entrenador2->id = string_from_format("PEPE");
	HiloEntrenadorPlanificable* entrenador2 = HiloEntrenadorPlanificableConstructor.new(_entrenador2);



	Entrenador * _entrenador3 = EntrenadorConstructor.new("2|4", "F|G|T", "A|B|C");
	free(_entrenador3->id);
	_entrenador3->id = string_from_format("SHAQUILLE");
	HiloEntrenadorPlanificable* entrenador3 = HiloEntrenadorPlanificableConstructor.new(_entrenador3);

	list_add(listaEntrenadores, entrenador);
	list_add(listaEntrenadores, entrenador2);
	list_add(listaEntrenadores, entrenador3);

	t_list* resultado = servicioDeadlockTest->procesarDeadlock(servicioDeadlockTest, listaEntrenadores);
	list_destroy_and_destroy_elements(resultado, destruirIntercambio);

	log_info(testLogger, "Deberia haber dos deadlocks: ASH y GANDALF. BROCK y ARAGORN");
	t_list* lista2 = list_create();


	Entrenador * _entrenador4 = EntrenadorConstructor.new("1|2", "X|Y|A", "X|Y|G"); // posicion, iniciales, objetivo.
	free(_entrenador4->id);
	_entrenador4->id = string_from_format("ASH");
	HiloEntrenadorPlanificable* entrenador4 = HiloEntrenadorPlanificableConstructor.new(_entrenador4);


	Entrenador * _entrenador5 = EntrenadorConstructor.new("2|4", "C", "B");
	free(_entrenador5->id);
	_entrenador5->id = string_from_format("BROCK");
	HiloEntrenadorPlanificable* entrenador5 = HiloEntrenadorPlanificableConstructor.new(_entrenador5);

	Entrenador * _entrenador6 = EntrenadorConstructor.new("2|4", "B", "C");
	free(_entrenador6->id);
	_entrenador6->id = string_from_format("ARAGORN");
	HiloEntrenadorPlanificable* entrenador6 = HiloEntrenadorPlanificableConstructor.new(_entrenador6);

	Entrenador * _entrenador7 = EntrenadorConstructor.new("2|4", "H|G", "A|H");
	free(_entrenador7->id);
	_entrenador7->id = string_duplicate("GANDALF");
	HiloEntrenadorPlanificable* entrenador7 = HiloEntrenadorPlanificableConstructor.new(_entrenador7);

	list_add(lista2, entrenador4);
	list_add(lista2, entrenador5);
	list_add(lista2, entrenador6);
	list_add(lista2, entrenador7);

	servicioDeadlockTest->primeraVez = true;
	t_list* resultado1 = servicioDeadlockTest->procesarDeadlock(servicioDeadlockTest, lista2);
	list_destroy_and_destroy_elements(resultado1, destruirIntercambio);

	log_info(testLogger, "Deberia haber tres deadlocks: ");

	t_list* lista3 = list_create();

	Entrenador * _e1 = EntrenadorConstructor.new("2|4", "A", "B");
	free(_e1->id);
	_e1->id = string_from_format("D1E1");
	HiloEntrenadorPlanificable* e1 = HiloEntrenadorPlanificableConstructor.new(_e1);

	Entrenador * _e2 = EntrenadorConstructor.new("2|4", "B", "C");
	free(_e2->id);
	_e2->id = string_from_format("D1E2");
	HiloEntrenadorPlanificable* e2 = HiloEntrenadorPlanificableConstructor.new(_e2);

	Entrenador * _e3 = EntrenadorConstructor.new("2|4", "C", "A");
	free(_e3->id);
	_e3->id = string_from_format("D1E3");
	HiloEntrenadorPlanificable* e3 = HiloEntrenadorPlanificableConstructor.new(_e3);

	Entrenador * _e4 = EntrenadorConstructor.new("2|4", "1", "2");
	free(_e4->id);
	_e4->id = string_from_format("circularE1");
	HiloEntrenadorPlanificable* e4 = HiloEntrenadorPlanificableConstructor.new(_e4);

	Entrenador * _e5 = EntrenadorConstructor.new("2|4", "2", "3");
	free(_e5->id);
	_e5->id = string_from_format("ciruclarE2");
	HiloEntrenadorPlanificable* e5 = HiloEntrenadorPlanificableConstructor.new(_e5);

	Entrenador * _e6 = EntrenadorConstructor.new("2|4", "3", "4");
	free(_e6->id);
	_e6->id = string_from_format("circukarE3");
	HiloEntrenadorPlanificable* e6 = HiloEntrenadorPlanificableConstructor.new(_e6);

	Entrenador * _e7 = EntrenadorConstructor.new("2|4", "4", "1");
	free(_e7->id);
	_e7->id = string_from_format("circularE4");
	HiloEntrenadorPlanificable* e7 = HiloEntrenadorPlanificableConstructor.new(_e7);

	Entrenador * _e8 = EntrenadorConstructor.new("2|4", "AA|CC|BB|ESTE", "AA|CC|BB|OTRO");
	free(_e8->id);
	_e8->id = string_from_format("D3E1");
	HiloEntrenadorPlanificable* e8 = HiloEntrenadorPlanificableConstructor.new(_e8);

	Entrenador * _e9 = EntrenadorConstructor.new("2|4", "C|OTRO", "C|ESTE");
	free(_e9->id);
	_e9->id = string_from_format("D3E2");
	HiloEntrenadorPlanificable* e9 = HiloEntrenadorPlanificableConstructor.new(_e9);

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

	Entrenador * _a1 = EntrenadorConstructor.new("2|4", "1|2", "6|5");
	free(_a1->id);
	_a1->id = string_from_format("a1");
	HiloEntrenadorPlanificable* a1 = HiloEntrenadorPlanificableConstructor.new(_a1);

	Entrenador * _a2 = EntrenadorConstructor.new("2|4", "5", "6");
	free(_a2->id);
	_a2->id = string_from_format("a2");
	HiloEntrenadorPlanificable* a2 = HiloEntrenadorPlanificableConstructor.new(_a2);

	Entrenador * _a3 = EntrenadorConstructor.new("2|4", "1|2|3", "4|5|2");
	free(_a3->id);
	_a3->id = string_from_format("a3");
	HiloEntrenadorPlanificable* a3 = HiloEntrenadorPlanificableConstructor.new(_a3);

	Entrenador * _a4 = EntrenadorConstructor.new("2|4", "4|3", "1|2");
	free(_a4->id);
	_a4->id = string_from_format("a4");
	HiloEntrenadorPlanificable* a4 = HiloEntrenadorPlanificableConstructor.new(_a4);

	Entrenador * _a5 = EntrenadorConstructor.new("2|4", "5|2", "3|1");
	free(_a5->id);
	_a5->id = string_from_format("a5");
	HiloEntrenadorPlanificable* a5 = HiloEntrenadorPlanificableConstructor.new(_a5);

	Entrenador * _a6 = EntrenadorConstructor.new("2|4", "6|6", "2|3");
	free(_a6->id);
	_a6->id = string_from_format("a6");
	HiloEntrenadorPlanificable* a6 = HiloEntrenadorPlanificableConstructor.new(_a6);

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
		HiloEntrenadorPlanificable* this = (HiloEntrenadorPlanificable*) elem;
		this->entrenador->destruir(this->entrenador);
		this->destruir(this);
	}
	list_destroy_and_destroy_elements(listaEntrenadores, killEntrenador);
	list_destroy_and_destroy_elements(lista2, killEntrenador);
	list_destroy_and_destroy_elements(lista3, killEntrenador);
	list_destroy_and_destroy_elements(lista4, killEntrenador);
}
