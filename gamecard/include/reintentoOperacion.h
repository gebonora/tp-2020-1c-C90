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

void iniciarListaSemaforosDeArchivo();

void agregarSemaforoALista(char* nombreArchivo);

void dumpListaSemaforosDeArchivo();

int esDirectorioPokemon(char* directorio);

#endif /* GAMECARD_INCLUDE_REINTENTOOPERACION_H_ */
