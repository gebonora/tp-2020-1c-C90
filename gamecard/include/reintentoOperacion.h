/*
 * reintentoOperacion.h
 *
 *  Created on: 22 jun. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_REINTENTOOPERACION_H_
#define GAMECARD_INCLUDE_REINTENTOOPERACION_H_

#include <pthread.h>

#include "fileSystem.h"

typedef struct {
	char* nombreArchivo;
	pthread_mutex_t mutexArchivo;
} FileMutex;

int puedeAccederAArchivo(char* nombreArchivo);

void cerrarArchivo(char* nombreArchivo);

void iniciarListaSemaforosDeArchivo();

void agregarSemaforoALista(char* nombreArchivo);

int obtenerPosicionEnLista(char* nombrePokemon);

void dumpListaSemaforosDeArchivo();

int esDirectorioPokemon(char* directorio);

int estaAbierto(char* nombrePokemon);

void freeListaSemaforos();

void freeFileMutex(void* elem);

#endif /* GAMECARD_INCLUDE_REINTENTOOPERACION_H_ */
