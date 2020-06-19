/*
 * manejoDeArchivos.c
 *
 *  Created on: 18 jun. 2020
 *      Author: utnso
 */

#include "manejoDeArchivos.h"

void crearPokemon(char* nombrePokemon) {
	char* rutaPokemon = crearRutaPokemon(nombrePokemon);
	mkdir(rutaPokemon, 0777);
	char* rutaMetadata = crearRutaMetadataPokemon(nombrePokemon);
	FILE* fp = fopen(rutaMetadata, "w+");

	fputs("DIRECTORY=N\n", fp);
	fputs("SIZE=0\n", fp);
	fputs("BLOCKS=[]\n", fp);
	fputs("OPEN=N", fp);
	fclose(fp);
	free(rutaPokemon);
	free(rutaMetadata);
}

int agregarCoordenadaPokemon(char* nombrePokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);

int leerClaveValorInt(char* subPath, char* clave) {
	char* path = crearRuta(subPath);
	if (!fileExists(path)) {
		free(path);
		return -1;
	}
	t_config* metadata = config_create(path);
	int retorno = config_get_int_value(metadata, clave);
	free(path);
	config_destroy(metadata);
	return retorno;
}

char* leerClaveValorString(char* subPath, char* clave) {
	char* path = crearRuta(subPath);
	if (!fileExists(path)) {
		free(path);
		return NULL;
	}
	t_config* metadata = config_create(path);
	char* retorno = string_new();
	string_append(&retorno, config_get_string_value(metadata, clave));
	free(path);
	config_destroy(metadata);
	return retorno;
}

int existePokemon(char* nombrePokemon) {
	char* rutaPokemon = crearRutaPokemon(nombrePokemon);
	int ret = fileExists(rutaPokemon);
	free(rutaPokemon);
	return ret;
}

int fileExists(char* path) {
	struct stat buffer;
	return (stat(path, &buffer) == 0);
}

char* crearRuta(char* subPath) {
	char* path = string_new();
	string_append_with_format(&path, "%s/%s", PUNTO_MONTAJE_TALLGRASS, subPath);
	return path;
}

char* crearRutaPokemon(char* nombrePokemon) {
	char* pathDirectorio = crearRuta("Files");
	char* filePokemon = string_new();
	string_append_with_format(&filePokemon, "%s/%s", pathDirectorio, nombrePokemon);
	free(pathDirectorio);
	return filePokemon;
}

char* crearRutaMetadataPokemon(char* nombrePokemon) {
	char* pathPokemon = crearRutaPokemon(nombrePokemon);
	char* fileMetadata = string_new();
	string_append_with_format(&fileMetadata, "%s/Metadata.bin", pathPokemon);
	free(pathPokemon);
	return fileMetadata;
}

void crearDirectoriosBase(char* subPath) {
	char* path = crearRuta(subPath);
	if (!fileExists(path)) {
		mkdir(path, 0777);
	}
	free(path);
}

void crearArchivosBase(char* subPath) {
	char* path = crearRuta(subPath);
	if (!fileExists(path)) {
		FILE* fp = fopen(path, "w");
		fclose(fp);
	}
	free(path);
}

void borrarDirectorioYContenido(char* subPath) {
	//vamos a usar shell
	char* ruta = crearRuta(subPath);
	char* shell = string_new();
	string_append_with_format(&shell, "%s%s", SHELL_COMMAND_DELETE, ruta);
	system(shell);
	free(ruta);
	free(shell);
}
