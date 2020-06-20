/*
 * bloques.h
 *
 *  Created on: 18 jun. 2020
 *      Author: utnso
 */

#ifndef GAMECARD_INCLUDE_BLOQUES_H_
#define GAMECARD_INCLUDE_BLOQUES_H_

#include "fileSystem.h"
#include "bitmap.h"

void crearBloques();
char* crearRutaBloque(int numeroDeBloque);
int asignarBloqueLibre();
void liberarBloque(int numeroDeBloque);
int calcularNumeroDeBloquesNecesarios(int size);

#endif /* GAMECARD_INCLUDE_BLOQUES_H_ */
