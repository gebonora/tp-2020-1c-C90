/*
 * bitmap.h
 *
 *  Created on: 18 jun. 2020
 *      Author: utnso
 */

#ifndef GAMECARD_INCLUDE_BITMAP_H_
#define GAMECARD_INCLUDE_BITMAP_H_

#include <commons/bitarray.h>

#include "manejoDeArchivos.h"
#include "fileSystem.h"

void crearBitmap();
int calcularTamanioBitArray(int bloques);
void cargarBitmap();
void dumpBitmap(t_bitarray* bitmap, int cantidadMostrada);
void cerrarBitmap();

char* l_mapeo;
int l_bitmapFD;

#endif /* GAMECARD_INCLUDE_BITMAP_H_ */
