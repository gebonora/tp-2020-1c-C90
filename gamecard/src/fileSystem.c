/*
 * fileSystem.c
 *
 *  Created on: 19 abr. 2020
 *      Author: GONZALO BONORA
 */

/* Paso inicial: Crear la estructura del Filesystem:
 * 					- Leer configs.
 * 					- Crear directorios necesarios para metadata .
 * 					- Implementar Logs.
 *
 * Segundo paso: Iniciar desarrollo de funciones del FS:
 * 					- Logica de creacion de directorios y guardado de informacion en metadata.
 * 					- Crear archivos pokemon vacios.
 *
 */

/* Dudas:
 *  1- El fs es single thread? Pareceria que si...
 *  2- Es un hilo para "Server Broker" y otro para "Server socket gameboy"?
 */
#include "fileSystem.h"

int main232() {
	puts("arranco el main");
	cargarConfigConexiones();
	//iniciarLoggers();
	crearDirectoriosBase("Metadata");
	crearDirectoriosBase("Files");
	crearArchivosBase("Metadata/Metadata.bin");
	crearArchivosBase("Metadata/Bitmap.bin");
	return 0;
}

// Al FS hay que proveerle un punto de montaje valido, a partir de ahi él crea toddo lo que necesita

void crearDirectoriosBase(char* subPath) {
	char* path = string_new();
	string_append_with_format(&path, "%s/%s", PUNTO_MONTAJE_TALLGRASS, subPath);
	if (!fileExists(path)) {
		mkdir(path, 0777);
	}
	free(path);
}

void crearArchivosBase(char* subPath) {
	char* path = string_new();
	string_append_with_format(&path, "%s/%s", PUNTO_MONTAJE_TALLGRASS, subPath);
	if (!fileExists(path)) {
		FILE* fp = fopen(path, "w");
		fclose(fp);
	}
	free(path);
}

int fileExists(char* path) { //check existence of files and directories
	struct stat buffer;
	return (stat(path, &buffer) == 0);
}

Pokemon* procesarNew(New* unNew) {
	char* nombreAppeared = unNew->pokemon->name->value;
	Coordinate* coordenadaAppeared = list_get(unNew->pokemon->coordinates, 0);
	uint32_t posXAppeared = coordenadaAppeared->pos_x;
	uint32_t posYAppeared = coordenadaAppeared->pos_y;
	//tengo que liberar la memoria del new afuera de esta funcion, y cuando haga el send, el create_pokemon se libera
	//ver que no haya un double free, que pasaria si los dos frees apuntan a lo mismo. capaz copiar el contenido del new para pasarle al create?

	//cosas de fileSystem acá...

	return create_pokemon(nombreAppeared, posXAppeared, posYAppeared);
}

Caught* procesarCatch(Pokemon* unPokemon) {
	uint32_t resultado;
	//acá el problema de New con la memoria no pasa, porque caught lleva solo un uint32_t

	//cosas de fileSystem acá...

	return create_caught_pokemon(resultado = 1);
}

Localized* procesarLocalized(Get* unGet) {
	char* nombreLocalized = unGet->name->value;
	//mismo problema que en New.

	//cosas de fileSystem acá. debería retornar un Pokemon* cargado con su lista de posiciones. Que retorna si no localizo nada? elements 0 y lista null?
	//Pokemon* unPokemon = fuciondeeFS

	//bloque para pruebas, aunque parte se usara en la funcion del fs
	Pokemon* unPokemon = malloc(sizeof(unPokemon));
	unPokemon->name->value = nombreLocalized; //ver memoria, pasar copiar quizas para que no choque con el Get*
	unPokemon->coordinates= list_create();
	list_add(unPokemon->coordinates,create_coordinate(2, 3));
	list_add(unPokemon->coordinates,create_coordinate(6, 7));

	//no hay funcion en libs para crear localized, lo creo a mano.
	Localized* localized = malloc(sizeof(localized));
	localized->pokemon = unPokemon;
	localized->coordinates_quantity = unPokemon->coordinates->elements_count;

	return localized;
}

