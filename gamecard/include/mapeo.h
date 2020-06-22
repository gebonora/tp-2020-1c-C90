/*
 * mapeo.h
 *
 *  Created on: 20 jun. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_MAPEO_H_
#define GAMECARD_INCLUDE_MAPEO_H_

#include "fileSystem.h"

char* mapearArchivoEnString(t_list* listaBloques, int tamanioArchivo);
void dumpearArchivo(t_list* listaBloques, char* archivoMapeado);

char* operarNew(char* archivoMapeado, int posX, int posY, int cantidad);
char* operarCatch(char* archivoMapeado, int posX, int posY);

char* obtenerValorEnString(char* string, char* clave);
char* obtenerClaveEnString(char* string);
Coordinate* obtenerCoordenadaDeString(char* string);

#endif /* GAMECARD_INCLUDE_MAPEO_H_ */
