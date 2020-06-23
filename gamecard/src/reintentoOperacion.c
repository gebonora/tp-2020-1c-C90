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
	char* rutaFiles = crearRuta(PATH_FILES);
	DIR* dirFiles = opendir(rutaFiles);
	struct dirent* dirPokemon;
	while ((dirPokemon = readdir(dirFiles))) {
		if (esDirectorioPokemon(dirPokemon->d_name)) {
			agregarSemaforoALista(dirPokemon->d_name);
			char* metadata = crearRutaMetadataPokemon(dirPokemon->d_name);
			setClaveValor(metadata,"OPEN","N");
			free(metadata);
		}
	}
	free(rutaFiles);
	closedir(dirFiles);
	dumpListaSemaforosDeArchivo();
}

void agregarSemaforoALista(char* nombreArchivo) {
	FileMutex* nuevaEntrada = malloc(sizeof(FileMutex));
	nuevaEntrada->nombreArchivo = string_duplicate(nombreArchivo);
	pthread_mutex_init(&(nuevaEntrada->mutexArchivo), NULL);
	list_add(listaSemaforos, nuevaEntrada);
}

int puedeAccederAArchivo(char* nombreArchivo) { //por fuera de esta funcion tenemos un while con el sleep de reintento operacion
	int resultado = 0;
	//espera por el mutex del archivo.
	FileMutex* fileMutex = list_get(listaSemaforos, obtenerPosicionEnLista(nombreArchivo));
	pthread_mutex_lock(&(fileMutex->mutexArchivo));

	if (estaAbierto(nombreArchivo)) {
		resultado = 0;
	} else {
		resultado = 1;
		char* path = crearRutaMetadataPokemon(nombreArchivo);
		setClaveValor(path, "OPEN", "Y");
		free(path);
	}
	//consulta el OPEN. si está en Y sale. return -1
	//					si esá en N lo graba como Y y entra. return 0

	//libera el semaforo
	pthread_mutex_unlock(&(fileMutex->mutexArchivo));
	return resultado;
}

void cerrarArchivo(char* nombreArchivo) {
	FileMutex* fileMutex = list_get(listaSemaforos, obtenerPosicionEnLista(nombreArchivo));
	char* path = crearRutaMetadataPokemon(nombreArchivo);
	pthread_mutex_lock(&(fileMutex->mutexArchivo));
	setClaveValor(path, "OPEN", "N");
	pthread_mutex_unlock(&(fileMutex->mutexArchivo));
	free(path);
}

void dumpListaSemaforosDeArchivo() {
	log_debug(loggerMain, "Semaforos: Inicializando...");
	for (int a = 0; a < listaSemaforos->elements_count; a++) {
		log_debug(loggerMain, "Se inicializo un semaforo para el archivo pokemon: '%s'.", ((FileMutex*) (list_get(listaSemaforos, a)))->nombreArchivo);
	}
	log_debug(loggerMain, "Listo!");
	puts("\n");
}

int obtenerPosicionEnLista(char* nombrePokemon) {
	for (int a = 0; a < listaSemaforos->elements_count; a++) {
		if (string_equals_ignore_case(nombrePokemon, ((FileMutex*) (list_get(listaSemaforos, a)))->nombreArchivo)) {
			return a;
		}
	}
	return -1; //no debería ocurrir nunca
}

int esDirectorioPokemon(char* directorio) {
	return !(string_equals_ignore_case(directorio, ".") || string_equals_ignore_case(directorio, "..") || string_equals_ignore_case(directorio, FILE_METADATA));
}

int estaAbierto(char* nombrePokemon) {
	char* rutaMetadata = crearRutaMetadataPokemon(nombrePokemon);
	char* valor = leerClaveValorString(rutaMetadata, "OPEN");
	int ret = string_equals_ignore_case(valor, "Y");
	free(rutaMetadata);
	free(valor);
	return ret;
}

void freeListaSemaforos() {
	list_destroy_and_destroy_elements(listaSemaforos, freeFileMutex);
}

void freeFileMutex(void* elem) {
	FileMutex* fileMutex = (FileMutex*) elem;
	pthread_mutex_destroy(&(fileMutex->mutexArchivo));
	free(fileMutex->nombreArchivo);
	free(fileMutex);
}

