/*
 * manejoDeArchivos.h
 *
 *  Created on: 18 jun. 2020
 *      Author: utnso
 */

#ifndef GAMECARD_INCLUDE_MANEJODEARCHIVOS_H_
#define GAMECARD_INCLUDE_MANEJODEARCHIVOS_H_

#include <commons/config.h>
#include <fileSystem.h>

void crearPokemon(char* nombrePokemon);
int agregarCoordenadaPokemon(char* nombrePokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);

int fileExists(char * path);
char* crearRuta(char* subPath);
void crearDirectoriosBase(char* subPath);
void crearArchivosBase(char* subPath);
char* crearRutaMetadataPokemon(char* nombrePokemon);
void borrarDirectorioYContenido(char* subPath);
char* crearRutaPokemon(char* nombrePokemon);
int existePokemon(char* nombrePokemon);

int leerClaveValorInt(char* subPath, char* clave);
char* leerClaveValorString(char* subPath, char* clave);

#endif /* GAMECARD_INCLUDE_MANEJODEARCHIVOS_H_ */
