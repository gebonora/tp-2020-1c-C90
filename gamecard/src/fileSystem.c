/*
 * fileSystem.c
 *
 *  Created on: 19 abr. 2020
 *      Author: GONZALO BONORA
 */
#include "fileSystem.h"
/*
 * -Al FS hay que proveerle un punto de montaje valido con un Metadata/Metadata.bin cargado con valores correctos.
 *
 * -Si se lee el argumento -format se formatea el disco, es decir se eliminan todos los archivos y directorios salvo Metadata/Metadata.bin
 * 		Luego se crean de nuevo, y se generan los bloques y un bitmap en 0, según lo leído en Metadata/Metadata.bin.
 *
 * -FileSystem en ejecución normal.
 *
 * */

int iniciarFileSystem(int argc, char* argv[]) { //de pruebas, luego sacar a iniciarFileSystem
	iniciarSemaforosFS();

	char* rutaMetadata = crearRuta(PATH_ARCHIVO_METADATA);
	if (!fileExists(rutaMetadata)) {
		log_error(loggerMain, "No se encontró un Metadata de FileSystem en el punto de montaje. Provea el archivo %s/Metadata/Metadata.bin"
				" y formatee ejecutando con -format. Abortando ejecución...", PUNTO_MONTAJE_TALLGRASS);
		return -1;
	} else {
		log_debug(loggerMain, "Metadata del FileSystem encontrada. Leyendo...");
	}
	free(rutaMetadata);
	leerMetadataFileSystem();

	if (argc == 2) {
		if (!strcmp(argv[1], "-format"))
			formatearTallGrass();
	}
	cargarBitmap();
	log_debug(loggerMain, "FileSystem iniciado!");
	puts("\n");
	return 0;
}

//FUNCIONES DE INICIALIZACION

void leerMetadataFileSystem() {
	char * path = crearRuta(PATH_ARCHIVO_METADATA);
	g_numberOfBlocks = leerClaveValorInt(path, "BLOCKS");
	log_debug(loggerMain, "Metadata: NUMBER OF BLOCKS: '%d'.", g_numberOfBlocks);
	g_blockSize = leerClaveValorInt(path, "BLOCK_SIZE");
	log_debug(loggerMain, "Metadata: BLOCK_SIZE: '%d'.", g_blockSize);
	char* magicNumber = leerClaveValorString(path, "MAGIC_NUMBER");
	log_debug(loggerMain, "Metadata: MAGIC NUMBER: '%s'.", magicNumber);
	free(magicNumber);
	free(path);

	log_debug(loggerMain, "Listo!");
	puts("\n");
}

void iniciarSemaforosFS() {
	pthread_mutex_init(&m_bitmap, NULL);
	iniciarListaSemaforosDeArchivo();
}

void cerrarSemaforosFS() {
	pthread_mutex_destroy(&m_bitmap);
	freeListaSemaforos();
}

//FUNCIONES DE FORMATEADOR

void formatearTallGrass() {
	log_debug(loggerMain, "Formateando el Disco...");
	borrarDirectorioYContenido("Blocks");
	borrarDirectorioYContenido("Files");
	char* pathBitmap = crearRuta(PATH_ARCHIVO_BITMAP);
	remove(pathBitmap);

	crearDirectoriosBase("Files");
	crearDirectoriosBase("Blocks");
	crearFilesMetadataDirectorio();
	crearBitmap();
	crearBloques();

	log_debug(loggerMain, "Listo!");
	puts("\n");
}

void crearFilesMetadataDirectorio() {
	crearArchivosBase("Files/Metadata.bin");
	char* path = crearRuta("Files/Metadata.bin");
	FILE* fp = fopen(path, "w+");
	fputs("DIRECTORY=Y", fp);
	fclose(fp);
	free(path);
}

//FUNCIONES DE PROCESADO DE REQUESTS

Pokemon* procesarNew(New* unNew, uint32_t idMensaje) {
	char* nombreAppeared = unNew->pokemon->name->value;
	Coordinate* coordenadaAppeared = list_get(unNew->pokemon->coordinates, 0);
	uint32_t posXAppeared = coordenadaAppeared->pos_x;
	uint32_t posYAppeared = coordenadaAppeared->pos_y;
	uint32_t cantidad = unNew->quantity;
//tengo que liberar la memoria del new afuera de esta funcion, y cuando haga el send, el create_pokemon se libera
//ver que no haya un double free, que pasaria si los dos frees apuntan a lo mismo. capaz copiar el contenido del new para pasarle al create?

	if (!existePokemon(nombreAppeared)) {
		crearPokemon(nombreAppeared);
		agregarSemaforoALista(nombreAppeared);
		log_debug(loggerMain, "Se inicializo un semaforo para el archivo pokemon: '%s'.", nombreAppeared);
	}

	//if abrirArchivo implementar semaforo.
	while (!puedeAccederAArchivo(nombreAppeared)) {
		pthread_mutex_lock(&m_loggerNew);
		log_warning(loggerNew, "El archivo: '%s' esta abierto. Reintentando la operacion New asociada al idMensaje: '%d' en '%d' segundos...", nombreAppeared, idMensaje,
				TIEMPO_DE_REINTENTO_OPERACION);
		pthread_mutex_unlock(&m_loggerNew);
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
	}

	agregarCoordenadaPokemon(nombreAppeared, posXAppeared, posYAppeared, cantidad);

	sleep(TIEMPO_RETARDO_OPERACION);
	cerrarArchivo(nombreAppeared);
	//cerrar archivo

	return create_pokemon(nombreAppeared, posXAppeared, posYAppeared);
}

Caught* procesarCatch(Pokemon* unPokemon, uint32_t idMensaje) {
	uint32_t resultado;
	char* nombreCaught = unPokemon->name->value;
	Coordinate* coordenadaCaught = list_get(unPokemon->coordinates, 0);
	uint32_t posXCaught = coordenadaCaught->pos_x;
	uint32_t posYCaught = coordenadaCaught->pos_y;
	//acá el problema de New con la memoria no pasa, porque caught lleva solo un uint32_t

	if (!existePokemon(nombreCaught)) {
		sleep(TIEMPO_RETARDO_OPERACION);
		return create_caught_pokemon(FAIL);
	}

	// if abrirArchivo

	while (!puedeAccederAArchivo(nombreCaught)) {
		pthread_mutex_lock(&m_loggerCatch);
		log_warning(loggerCatch, "El archivo: '%s' esta abierto. Reintentando la operacion Catch asociada al idMensaje: '%d' en '%d' segundos...", nombreCaught, idMensaje,
				TIEMPO_DE_REINTENTO_OPERACION);
		pthread_mutex_unlock(&m_loggerCatch);
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
	}

	//cosas de fileSystem acá...
	resultado = quitarCoordenadaPokemon(nombreCaught, posXCaught, posYCaught);

	sleep(TIEMPO_RETARDO_OPERACION);
	cerrarArchivo(nombreCaught);
	//cerrar archivo

	return create_caught_pokemon(resultado);
}

Localized* procesarLocalized(Get* unGet, uint32_t idMensaje) {
	char* nombreLocalized = unGet->name->value;
	Localized* localized = malloc(sizeof(Localized));
	//mismo problema que en New.

	puts(nombreLocalized);
	if (!existePokemon(nombreLocalized)) {
		Pokemon* pokemonNulo = malloc(sizeof(Pokemon));
		pokemonNulo->name = create_name(nombreLocalized);
		pokemonNulo->coordinates = list_create();
		localized->pokemon = pokemonNulo;
		localized->coordinates_quantity = pokemonNulo->coordinates->elements_count; //DEBERIA SER 0
		sleep(TIEMPO_RETARDO_OPERACION);
		return localized;
	}

	//abrir archivo

	while (!puedeAccederAArchivo(nombreLocalized)) {
		pthread_mutex_lock(&m_loggerGet);
		log_warning(loggerGet, "El archivo: '%s' esta abierto. Reintentando la operacion Get asociada al idMensaje: '%d' en '%d' segundos...", nombreLocalized, idMensaje,
				TIEMPO_DE_REINTENTO_OPERACION);
		pthread_mutex_unlock(&m_loggerGet);
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
	}

	//cosas de fileSystem acá. debería retornar un Pokemon* cargado con su lista de posiciones. Que retorna si no localizo nada? elements 0 y lista null?
	Pokemon* pokemonLocalized = obtenerCoordenadasPokemon(nombreLocalized);

	sleep(TIEMPO_RETARDO_OPERACION);
	cerrarArchivo(nombreLocalized);
	//cerrarArchivo;
	localized->pokemon = pokemonLocalized;
	localized->coordinates_quantity = pokemonLocalized->coordinates->elements_count;

	return localized;
}

