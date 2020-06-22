/*
 * ReintentoOperacion.c
 *
 *  Created on: 22 jun. 2020
 *      Author: GONZALO BONORA
 */
#include "reintentoOperacion.h"

void iniciarListaSemaforosDeArchivo() {
	listaSemaforos = list_create();
	//leer el directorio Files, guardando en lista el nombre y el semaforo inicializado.
	char* rutaFiles = crearRuta("Files");
	DIR* dirFiles = opendir(rutaFiles);
	struct dirent* dirPokemon;
	while ((dirPokemon = readdir(dirFiles))) {
		if (esDirectorioPokemon(dirPokemon->d_name)) {
			FileMutex* nuevaEntrada = malloc(sizeof(FileMutex));
			nuevaEntrada->nombreArchivo = string_duplicate(dirPokemon->d_name);
			pthread_mutex_init(&(nuevaEntrada->mutexArchivo), NULL);
			list_add(listaSemaforos, nuevaEntrada);
		}
	}
	free(rutaFiles);
	closedir(dirFiles);
	dumpListaSemaforosDeArchivo();
}

void agregarSemaforoALista(char* nombreArchivo) {
	//agregar a la lista una entrada con el nombre y el semaforo, inicializandolo.
}

int accederAArchivo(char* nombreArchivo) { //por fuera de esta funcion tenemos un while con el sleep de reintento operacion
	int resultado = -1;
	//espera por el mutex del archivo.

	//consulta el OPEN. si está en Y sale. return -1
	//					si esá en N lo graba como Y y entra. return 0

	//libera el semaforo
	return resultado;
}

void dumpListaSemaforosDeArchivo() {
	log_info(loggerMain, "Semaforos: Inicializando...");
	for (int a = 0; a < listaSemaforos->elements_count; a++) {
		log_info(loggerMain, "Se inicializo un semaforo para el archivo pokemon: '%s'.", ((FileMutex*) (list_get(listaSemaforos, a)))->nombreArchivo);
	}
	log_info(loggerMain, "Listo!");
	puts("\n");
}

int esDirectorioPokemon(char* directorio) {
	return !(string_equals_ignore_case(directorio, ".") || string_equals_ignore_case(directorio, "..") || string_equals_ignore_case(directorio, "Metadata.bin"));

}
