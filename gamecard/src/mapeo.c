/*
 * mapeo.c
 *
 *  Created on: 20 jun. 2020
 *      Author: utnso
 */
#include "mapeo.h"

//FUNCIONES PARA LECTURA/ESCRITURA DE ARCHIVOS POKEMON

char* mapearArchivoEnString(t_list* listaBloques, int tamanioArchivo) {
	printf("tamanioarchivo metadata: %d\n", tamanioArchivo);

	char* archivoMapeado = string_new();
	int leido = 0;
	for (int a = 0; a < listaBloques->elements_count; a++) {
		int bloque = atoi(list_get(listaBloques, a));
		char* rutaBloque = crearRutaBloque(bloque);

		int lecturaActual = tamanioArchivo - leido < g_blockSize ? tamanioArchivo - leido : g_blockSize;

		FILE* fp = fopen(rutaBloque, "r+");
		char* blockAsCharPtr = malloc(sizeof(char) * lecturaActual);
		int leidoPorRead = fread(blockAsCharPtr, lecturaActual, 1, fp);

		char* subStringAux = string_substring(blockAsCharPtr, 0, lecturaActual);
		string_append(&archivoMapeado, subStringAux); //substring para eliminar los bytes basura que puede leer.
		leido += lecturaActual;

		fclose(fp);
		free(blockAsCharPtr);
		free(rutaBloque);
		free(subStringAux);
		printf("leido por fread: %d\n", leidoPorRead);
	}
	//puts("archivoLeido:");
	//puts(archivoMapeado);

	printf("tamanio leido final: %d, variableLeido: %d\n", string_length(archivoMapeado), leido);

	return archivoMapeado;
}

void dumpearArchivo(t_list* listaBloques, char* archivoMapeado) {
	//esta funcion recibe los bloques necesarios y ya asignados, y dumpea el archivoMapeado.
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

		truncate(rutaBloque,g_blockSize);

		free(rutaBloque);
	}
	printf("tamanio escrito : %d\n", desplazamiento);
}

//FUNCIONES DE PROCESAMIENTO DE OPERACIONES

char* operarNew(char* archivoMapeado, int posX, int posY, int cantidad) {

	puts("archivo que llega a operar");
	puts(archivoMapeado);
	puts("tamanio archivo;");
	printf("%d\n", string_length(archivoMapeado));

	char* clave = string_from_format("%d-%d", posX, posY);
	char* retorno = string_new();
	if (!string_contains(archivoMapeado, clave)) {
		string_append_with_format(&retorno, "%s%s=%d\n", archivoMapeado, clave, cantidad);
		free(archivoMapeado);
		free(clave);
		return retorno;
	}
	char ** arrAux = string_split(archivoMapeado, "\n");	//cambiar esto???
	int a = 0;

	while (arrAux[a] != NULL) {
		a++;
	}

	printf("TAMANIO ARR: %d", a);
	a = 0;

	while (arrAux[a] != NULL) { //buscar la coordenada y editar
		if (string_starts_with(arrAux[a], clave)) {
			char* valor = obtenerValorEnString(arrAux[a], clave);
			int cantidadNueva = cantidad + atoi(valor);
			free(arrAux[a]); //reasignamos a un string nuevo
			arrAux[a] = string_from_format("%s=%d", clave, cantidadNueva);
			printf("size del string modificado: %d, strleng del mismo; %d\n", string_length(arrAux[a]), strlen(arrAux[a]));
			free(valor);
			break;
		}
		a++;
	}
	a = 0;
	//puts("operando...");
	while (arrAux[a] != NULL) {
		if (!string_equals_ignore_case(arrAux[a], "\n")) {
			string_append_with_format(&retorno, "%s\n", arrAux[a]);
			//puts(arrAux[a]); // ccuando llega al 10 se agrega uno vacio???
		}
		a++;
	}
	freeArrayChars(arrAux);
	free(clave);
	free(archivoMapeado);
	puts("archivo procesado:");
	puts(retorno);
	printf("tamanio procesado:\n%d", string_length(retorno));
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

