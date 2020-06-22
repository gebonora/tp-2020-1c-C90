/*
 * tests.c
 *
 *  Created on: 21 jun. 2020
 *      Author: GONZALO BONORA
 */
#include "tests.h"

void testCreacionCoordenadasMultiplesPokemon(char* nombrePokemon, int cantidadIteraciones) {

	for (int a = 0; a < cantidadIteraciones; a++) {
		New* newTest = create_new_pokemon(nombrePokemon, a, 2, 1);
		Pokemon* res = procesarNew(newTest);
		free_new(newTest);
		free_pokemon(res);
	}
}

void testCreacionCoordenadaPokemon(char* nombrePokemon, int cantidadIteraciones, int corX, int corY) {
	for (int a = 0; a < cantidadIteraciones; a++) {
		New* newTest = create_new_pokemon(nombrePokemon, corX, corY, 1);
		Pokemon* res = procesarNew(newTest);
		free_new(newTest);
		free_pokemon(res);
	}
}

void testRemocionCoordenadaPokemon(char* nombrePokemon, int cantidadIteraciones, int corX, int corY) {

	for (int a = 0; a < cantidadIteraciones; a++) {
		Pokemon* catchTest = create_pokemon(nombrePokemon, corX, corY);
		Caught* res = procesarCatch(catchTest);
		free_pokemon(catchTest);
		free(res);
	}
}

