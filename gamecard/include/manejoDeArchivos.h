/*
 * manejoDeArchivos.h
 *
 *  Created on: 18 jun. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_MANEJODEARCHIVOS_H_
#define GAMECARD_INCLUDE_MANEJODEARCHIVOS_H_

#include <commons/config.h>
#include <fileSystem.h>

void crearPokemon(char* nombrePokemon);
int agregarCoordenadaPokemon(char* nombrePokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);

int quitarCoordenadaPokemon(char* nombrePokemon, uint32_t posX, uint32_t posY);

Pokemon* obtenerCoordenadasPokemon(char* nombrePokemon);

int fileExists(char * path);
char* crearRuta(char* subPath);
void crearDirectoriosBase(char* subPath);
void crearArchivosBase(char* subPath);
char* crearRutaMetadataPokemon(char* nombrePokemon);
void borrarDirectorioYContenido(char* subPath);
char* crearRutaPokemon(char* nombrePokemon);
int existePokemon(char* nombrePokemon);

int leerClaveValorInt(char* path, char* clave);
char* leerClaveValorString(char* path, char* clave);
char** leerClaveValorArray(char* path, char* clave);
t_list* leerClaveValorList(char* path, char* clave);
char* stringDesdeLista(t_list* lista);
void freeArrayChars(char** arr);
void freeElem(void* elem);
int setClaveValor(char* path, char* clave, char* valor);

#endif /* GAMECARD_INCLUDE_MANEJODEARCHIVOS_H_ */
