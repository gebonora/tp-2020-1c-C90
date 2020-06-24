/*
 * mapeo.c
 *
 *  Created on: 20 jun. 2020
 *      Author: utnso
 */
#include "mapeo.h"

//FUNCIONES PARA LECTURA/ESCRITURA DE ARCHIVOS POKEMON

char* mapearArchivoEnString(t_list* listaBloques, int tamanioArchivo) {
	char* archivoMapeado = string_new();
	int leido = 0;

	for (int a = 0; a < listaBloques->elements_count; a++) {
		int bloque = atoi(list_get(listaBloques, a));
		char* rutaBloque = crearRutaBloque(bloque);

		int lecturaActual = tamanioArchivo - leido < g_blockSize ? tamanioArchivo - leido : g_blockSize;

		FILE* fp = fopen(rutaBloque, "r+");
		char* blockAsCharPtr = malloc(sizeof(char) * lecturaActual);
		fread(blockAsCharPtr, lecturaActual, 1, fp);

		char* subStringAux = string_substring(blockAsCharPtr, 0, lecturaActual);
		string_append(&archivoMapeado, subStringAux); //substring para eliminar los bytes basura que puede leer.
		leido += lecturaActual;

		fclose(fp);
		free(blockAsCharPtr);
		free(rutaBloque);
		free(subStringAux);
	}
	return archivoMapeado;
}

void dumpearArchivo(t_list* listaBloques, char* archivoMapeado) {
	//Esta funcion recibe los bloques necesarios ya asignados y dumpea en ellos el archivoMapeado.
	int desplazamiento = 0;
	int tamanioArchivo = string_length(archivoMapeado);

	for (int a = 0; a < listaBloques->elements_count; a++) {
		int bloque = atoi(list_get(listaBloques, a));
		char*rutaBloque = crearRutaBloque(bloque);

		int escrituraActual = tamanioArchivo - desplazamiento < g_blockSize ? tamanioArchivo - desplazamiento : g_blockSize;

		FILE* fp = fopen(rutaBloque, "w");
		fwrite(archivoMapeado + desplazamiento, escrituraActual, 1, fp);
		desplazamiento += escrituraActual;

		fclose(fp);

		truncate(rutaBloque, g_blockSize);

		free(rutaBloque);
	}
}

//FUNCIONES DE PROCESAMIENTO DE OPERACIONES

char* operarNew(char* archivoMapeado, int posX, int posY, int cantidad) {
	char* clave = string_from_format("%d-%d", posX, posY);
	char* retorno = string_new();

	if (!string_contains(archivoMapeado, clave)) {
		string_append_with_format(&retorno, "%s%s=%d\n", archivoMapeado, clave, cantidad);
		free(archivoMapeado);
		free(clave);
		return retorno;
	}

	char ** arrAux = string_split(archivoMapeado, "\n");

	int a = 0;
	while (arrAux[a] != NULL) {
		if (string_starts_with(arrAux[a], clave)) {
			char* valor = obtenerValorEnString(arrAux[a], clave);
			int cantidadNueva = cantidad + atoi(valor);
			free(arrAux[a]); //reasignamos a un string nuevo
			arrAux[a] = string_from_format("%s=%d", clave, cantidadNueva);
			free(valor);
			break;
		}
		a++;
	}

	a = 0;
	while (arrAux[a] != NULL) {
		if (!string_equals_ignore_case(arrAux[a], "\n")) {
			string_append_with_format(&retorno, "%s\n", arrAux[a]);
		}
		a++;
	}

	freeArrayChars(arrAux);
	free(clave);
	free(archivoMapeado);
	return retorno;
}

char* operarCatch(char* archivoMapeado, int posX, int posY) {
	char* clave = string_from_format("%d-%d", posX, posY);
	char* retorno = string_new();

	char ** arrAux = string_split(archivoMapeado, "\n");

	int a = 0;
	while (arrAux[a] != NULL) {
		if (string_starts_with(arrAux[a], clave)) {
			char* valor = obtenerValorEnString(arrAux[a], clave);
			int cantidadNueva = atoi(valor) - 1;
			if (cantidadNueva == 0) {
				free(arrAux[a]);
				arrAux[a] = string_new(); //marco como vacio
			} else {
				free(arrAux[a]); //reasignamos a un string nuevo
				arrAux[a] = string_from_format("%s=%d", clave, cantidadNueva);
			}
			free(valor);
		}
		a++;
	}

	a = 0;
	while (arrAux[a] != NULL) {
		if (!string_is_empty(arrAux[a])) { //filtro por vacio
			string_append_with_format(&retorno, "%s\n", arrAux[a]);
		}
		a++;
	}

	freeArrayChars(arrAux);
	free(clave);
	free(archivoMapeado);
	return retorno;
}

char* obtenerValorEnString(char* string, char* clave) {
	return string_substring(string, string_length(clave) + 1, string_length(string) - string_length(clave) - 1);
}

char* obtenerClaveEnString(char* string) {
	char** arr = string_split(string, "=");
	char* ret = string_duplicate(arr[0]);
	freeArrayChars(arr);
	return ret;
}

Coordinate* obtenerCoordenadaDeString(char* string) {
	char* clave = obtenerClaveEnString(string);
	Coordinate* coor = malloc(sizeof(Coordinate));
	char ** arr = string_split(clave, "-");
	uint32_t x = atoi(arr[0]);
	uint32_t y = atoi(arr[1]);
	coor->pos_x = x;
	coor->pos_y = y;
	freeArrayChars(arr);
	free(clave);
	return coor;

}

