/*
 * tests.c
 *
 *  Created on: 21 jun. 2020
 *      Author: GONZALO BONORA
 */
#include "tests.h"

void correrTests() {
	testCreacionCoordenadasMultiplesPokemon("pikachu", 30);
	testCreacionCoordenadaPokemon("charmander", 1, 1, 2);
	testRemocionCoordenadaPokemon("charmander", 1, 1, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 0, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 1, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 2, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 3, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 4, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 5, 2);
	testGetPokemon("pikachu");
	testGetPokemon("charmander");

}

void testCreacionCoordenadasMultiplesPokemon(char* nombrePokemon, int cantidadIteraciones) {

	for (int a = 0; a < cantidadIteraciones; a++) {
		New* newTest = create_new_pokemon(nombrePokemon, a, 2, 1);
		Pokemon* res = procesarNew(newTest, a);
		free_new(newTest);
		free_pokemon(res);
	}
}

void testCreacionCoordenadaPokemon(char* nombrePokemon, int cantidadIteraciones, int corX, int corY) {

	for (int a = 0; a < cantidadIteraciones; a++) {
		New* newTest = create_new_pokemon(nombrePokemon, corX, corY, 1);
		Pokemon* res = procesarNew(newTest, a);
		free_new(newTest);
		free_pokemon(res);
	}
}

void testRemocionCoordenadaPokemon(char* nombrePokemon, int cantidadIteraciones, int corX, int corY) {

	for (int a = 0; a < cantidadIteraciones; a++) {
		Pokemon* catchTest = create_pokemon(nombrePokemon, corX, corY);
		Caught* res = procesarCatch(catchTest, a);
		free_pokemon(catchTest);
		free(res);
	}
}

void testGetPokemon(char* nombrePokemon) {

	Get* getTest = create_get_pokemon(nombrePokemon);
	Localized* res = procesarLocalized(getTest, 420);
	free_get(getTest);
	free_localized(res);
}

void testAbrirArchivo(char* nombrePokemon) {

	if (puedeAccederAArchivo(nombrePokemon))
		puts("SI");
	else
		puts("NO");
}

