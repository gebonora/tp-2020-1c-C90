/*
 * mapeo.c
 *
 *  Created on: 20 jun. 2020
 *      Author: utnso
 */
#include "mapeo.h"

//FUNCIONES PARA LECTURA/ESCRITURA DE ARCHIVOS POKEMON

char* mapearArchivoEnString(t_list* listaBloques) {
	char* archivoMapeado = string_new();
	for (int a = 0; a < listaBloques->elements_count; a++) {
		int bloque = atoi(list_get(listaBloques, a));
		char* rutaBloque = crearRutaBloque(bloque);

		FILE* fp = fopen(rutaBloque, "r+");
		char* blockAsCharPtr = malloc(sizeof(char) * g_blockSize);
		fread(blockAsCharPtr, g_blockSize, 1, fp); //meterle un leido hasta el size para no leer de mas
		string_append(&archivoMapeado, blockAsCharPtr);

		fclose(fp);
		free(blockAsCharPtr);
		free(rutaBloque);
	}
	return archivoMapeado;
}

void dumpearArchivo(t_list* listaBloques, char* archivoMapeado) {
	//esta funcion recibe los bloques necesarios y ya asignados, y dumpea el archivoMapeado.
	int desplazamiento = 0;
	for (int a = 0; a < listaBloques->elements_count; a++) {
		int bloque = atoi(list_get(listaBloques, a));
		char*rutaBloque = crearRutaBloque(bloque);
		puts(rutaBloque);

		FILE* fp = fopen(rutaBloque, "w");
		fwrite(archivoMapeado + desplazamiento, g_blockSize, 1, fp);
		desplazamiento += g_blockSize;

		fclose(fp);
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
		}
		a++;
	}
	a = 0;
	while (arrAux[a] != NULL) {
		string_append_with_format(&retorno, "%s\n", arrAux[a]);
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
			a++;
		}
	}
	freeArrayChars(arrAux);
	free(clave);
	free(archivoMapeado);
	return retorno;
}

char* obtenerValorEnString(char* string, char* clave) {
	return string_substring(string, string_length(clave) + 1, string_length(string) - string_length(clave) - 1);
}

