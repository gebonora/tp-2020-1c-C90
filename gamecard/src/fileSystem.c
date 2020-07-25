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

//PUNTO DE ENTRADA AL FILESYSTEM
int iniciarFileSystem(int argc, char* argv[]) { //de pruebas, luego sacar a iniciarFileSystem

	char* rutaMetadata = crearRuta(PATH_ARCHIVO_METADATA);
	if (!fileExists(rutaMetadata)) {
		log_error(loggerMain, "No se encontró un Metadata de FileSystem en el punto de montaje. Provea el archivo %s/Metadata/Metadata.bin"
				" y formatee ejecutando con -format. Abortando ejecución...", PUNTO_MONTAJE_TALLGRASS);
		free(rutaMetadata);
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

	iniciarSemaforosFS();
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

//FUNCIONES DEL FORMATEADOR

void formatearTallGrass() {
	log_debug(loggerMain, "Formateando el Disco...");
	borrarDirectorioYContenido("Blocks");
	borrarDirectorioYContenido("Files");
	char* pathBitmap = crearRuta(PATH_ARCHIVO_BITMAP);
	remove(pathBitmap);
	free(pathBitmap);
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

	//Si no existe el archivo Pokemon, lo creamos y agregamos su semáforo a la lista global.
	if (!existePokemon(nombreAppeared)) {
		crearPokemon(nombreAppeared);
		agregarSemaforoALista(nombreAppeared);
		log_debug(loggerMain, "Se creó un archivo pokemón: '%s'.", nombreAppeared);
		log_debug(loggerMain, "Se inicializó un semáforo para el archivo pokemón: '%s'.", nombreAppeared);
	}

	//Intentamos acceder al archivo. Hay un semáforo por archivo metadata para controlar el acceso y evitar condiciones de carrera. No hay espera activa porque tenemos un Sleep.
	while (!puedeAccederAArchivo(nombreAppeared)) {
		pthread_mutex_lock(&m_loggerNew);
		log_warning(loggerNew, "El archivo pokemón: '%s' está abierto. Reintentando la operación New asociada al idMensaje: '%d' en '%d' segundos...", nombreAppeared,
				idMensaje, TIEMPO_DE_REINTENTO_OPERACION);
		pthread_mutex_unlock(&m_loggerNew);
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
	}
	pthread_mutex_lock(&m_loggerNew);
	log_debug(loggerNew, "El archivo pokemón: '%s' fue abierto por la operación New asociada al idMensaje: '%d'", nombreAppeared, idMensaje);
	pthread_mutex_unlock(&m_loggerNew);

	//Procesamiento del FileSystem. Escribe y puede asignar bloques.
	if (agregarCoordenadaPokemon(nombreAppeared, posXAppeared, posYAppeared, cantidad) < 0) {
		pthread_mutex_lock(&m_loggerNew);
		log_error(loggerNew, "No hay suficientes bloques libres para realizar la operación New asociada al idMensaje: '%d'", idMensaje);
		pthread_mutex_unlock(&m_loggerNew);
	}

	//Sleep pedido por requerimiento y cerramos archivo.
	sleep(TIEMPO_RETARDO_OPERACION);
	cerrarArchivo(nombreAppeared);
	pthread_mutex_lock(&m_loggerNew);
	log_debug(loggerNew, "El archivo pokemón: '%s' fue cerrado por la operación New asociada al idMensaje: '%d'", nombreAppeared, idMensaje);
	pthread_mutex_unlock(&m_loggerNew);

	return create_pokemon(nombreAppeared, posXAppeared, posYAppeared);
}

Caught* procesarCatch(Pokemon* unPokemon, uint32_t idMensaje) {
	uint32_t resultado;
	char* nombreCaught = unPokemon->name->value;
	Coordinate* coordenadaCaught = list_get(unPokemon->coordinates, 0);
	uint32_t posXCaught = coordenadaCaught->pos_x;
	uint32_t posYCaught = coordenadaCaught->pos_y;

	//Si no existe el archivo Pokemon, se hace el Sleep requerido y se retorna FAIL.
	if (!existePokemon(nombreCaught)) {
		sleep(TIEMPO_RETARDO_OPERACION);
		return create_caught_pokemon(FAIL);
	}

	//Intentamos acceder al archivo. Hay un semáforo por archivo metadata para controlar el acceso y evitar condiciones de carrera. No hay espera activa porque tenemos un Sleep.
	while (!puedeAccederAArchivo(nombreCaught)) {
		pthread_mutex_lock(&m_loggerCatch);
		log_warning(loggerCatch, "El archivo pokemón: '%s' está abierto. Reintentando la operación Catch asociada al idMensaje: '%d' en '%d' segundos...", nombreCaught,
				idMensaje, TIEMPO_DE_REINTENTO_OPERACION);
		pthread_mutex_unlock(&m_loggerCatch);
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
	}

	pthread_mutex_lock(&m_loggerCatch);
	log_debug(loggerCatch, "El archivo pokemón: '%s' fue abierto por la operación Catch asociada al idMensaje: '%d'", nombreCaught, idMensaje);
	pthread_mutex_unlock(&m_loggerCatch);

	//Procesamiento del FileSystem. Lee y puede desasignar bloques. Se trata de quitar la coordenada y obtiene un resultado acorde.
	resultado = quitarCoordenadaPokemon(nombreCaught, posXCaught, posYCaught);

	//Sleep pedido por requerimiento y cerramos archivo.
	sleep(TIEMPO_RETARDO_OPERACION);
	cerrarArchivo(nombreCaught);

	pthread_mutex_lock(&m_loggerCatch);
	log_debug(loggerCatch, "El archivo pokemón: '%s' fue cerrado por la operación Catch asociada al idMensaje: '%d'", nombreCaught, idMensaje);
	pthread_mutex_unlock(&m_loggerCatch);

	return create_caught_pokemon(resultado);
}

Localized* procesarLocalized(Get* unGet, uint32_t idMensaje) {
	char* nombreLocalized = unGet->name->value;
	Localized* localized = malloc(sizeof(Localized));

	//Si no existe el archivo Pokemon, se hace el Sleep requerido y retorna un localized con lista vacía.
	if (!existePokemon(nombreLocalized)) {
		Pokemon* pokemonNulo = malloc(sizeof(Pokemon));
		pokemonNulo->name = create_name(nombreLocalized);
		pokemonNulo->coordinates = list_create();
		localized->pokemon = pokemonNulo;
		localized->coordinates_quantity = pokemonNulo->coordinates->elements_count; //DEBERIA SER 0
		sleep(TIEMPO_RETARDO_OPERACION);
		return localized;
	}

	//Intentamos acceder al archivo. Hay un semáforo por archivo metadata para controlar el acceso y evitar condiciones de carrera. No hay espera activa porque tenemos un Sleep.
	while (!puedeAccederAArchivo(nombreLocalized)) {
		pthread_mutex_lock(&m_loggerGet);
		log_warning(loggerGet, "El archivo: '%s' está abierto. Reintentando la operacion Get asociada al idMensaje: '%d' en '%d' segundos...", nombreLocalized, idMensaje,
				TIEMPO_DE_REINTENTO_OPERACION);
		pthread_mutex_unlock(&m_loggerGet);
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
	}

	pthread_mutex_lock(&m_loggerGet);
	log_debug(loggerGet, "El archivo pokemón: '%s' fue abierto por la operación Get asociada al idMensaje: '%d'", nombreLocalized, idMensaje);
	pthread_mutex_unlock(&m_loggerGet);

	//Procesamiento de FileSystem. Lee bloques. Obtiene una lista de coordenadas, vacía si no hay.
	Pokemon* pokemonLocalized = obtenerCoordenadasPokemon(nombreLocalized);

	//Sleep pedido por requerimiento y cerramos archivo.
	sleep(TIEMPO_RETARDO_OPERACION);
	cerrarArchivo(nombreLocalized);

	pthread_mutex_lock(&m_loggerGet);
	log_debug(loggerGet, "El archivo pokemón: '%s' fue cerrado por la operación Get asociada al idMensaje: '%d'", nombreLocalized, idMensaje);
	pthread_mutex_unlock(&m_loggerGet);

	localized->pokemon = pokemonLocalized;
	localized->coordinates_quantity = pokemonLocalized->coordinates->elements_count;
	return localized;
}

