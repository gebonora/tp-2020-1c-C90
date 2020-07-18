/*
 * tests.h
 *
 *  Created on: 21 jun. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_TESTS_H_
#define GAMECARD_INCLUDE_TESTS_H_

#include "fileSystem.h"

void correrTests();

void testCreacionCoordenadasMultiplesPokemon(char* nombrePokemon, int cantidadIteraciones);

void testCreacionCoordenadaPokemon(char* nombrePokemon, int cantidadIteraciones, int corX, int corY);

void testRemocionCoordenadaPokemon(char* nombrePokemon, int cantidadIteraciones, int corX, int corY);

void testGetPokemon(char* nombrePokemon);

void testAbrirArchivo(char* nombrePokemon);

#endif /* GAMECARD_INCLUDE_TESTS_H_ */
