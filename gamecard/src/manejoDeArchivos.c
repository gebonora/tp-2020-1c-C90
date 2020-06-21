/*
 * manejoDeArchivos.c
 *
 *  Created on: 18 jun. 2020
 *      Author: GONZALO BONORA
 */

#include "manejoDeArchivos.h"

//FUNCIONES PARA EL FLUJO NEW/APPEARED

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

int agregarCoordenadaPokemon(char* nombrePokemon, uint32_t posX, uint32_t posY, uint32_t cantidad) {
	char* rutaMetadata = crearRutaMetadataPokemon(nombrePokemon);
	t_list* listaBloques = leerClaveValorList(rutaMetadata, "BLOCKS");
	int size = leerClaveValorInt(rutaMetadata, "SIZE");
	char* archivoMapeado;

	/*
	 * si size = 0 -> lista = {data}
	 * else creamos a lista leyendo y procesamos el new:
	 * dumpear lista  en los bloques:
	 * 1ro: calcular los bloques necesarios, asignar si hace falta.
	 * 2do: armar un gran char* con toda la info serializada.
	 * 3ro: escribir de a BLOCK_SIZE en el bloque, con un desplazamiento acumulado.
	 * size = desplazamiento acumulado.
	 * blocks = actualizar con bloques usados(ver el char**)
	 *
	 */
	if (size == 0) { //arreglar, cambiado para test
		archivoMapeado = string_from_format("%d-%d=%d", posX, posY, cantidad);
	} else {
		archivoMapeado = mapearArchivoEnString(listaBloques);
		archivoMapeado = operarNew(archivoMapeado, posX, posY, cantidad);
	}

	int nuevoTamanio = string_length(archivoMapeado);
	int cantidadBloquesRequeridos = calcularNumeroDeBloquesNecesarios(nuevoTamanio);

	while (listaBloques->elements_count < cantidadBloquesRequeridos) {
		int bloqueNuevo = asignarBloqueLibre();
		list_add(listaBloques, string_itoa(bloqueNuevo));
	}

	dumpearArchivo(listaBloques, archivoMapeado);

	char* blocksMetadata = stringDesdeLista(listaBloques);
	setClaveValor(rutaMetadata, "BLOCKS", blocksMetadata);
	free(blocksMetadata);

	char* nuevoTamanioAsString = string_itoa(nuevoTamanio);
	setClaveValor(rutaMetadata, "SIZE", nuevoTamanioAsString);
	free(nuevoTamanioAsString);

	free(archivoMapeado);
	free(rutaMetadata);
	list_destroy_and_destroy_elements(listaBloques, freeElem);

	return 0;
}

//FUNCIONES PARA EL FLUJO CATCH/CAUGHT

int quitarCoordenadaPokemon(char* nombrePokemon, uint32_t posX, uint32_t posY) {

	char* rutaMetadata = crearRutaMetadataPokemon(nombrePokemon);

	int size = leerClaveValorInt(rutaMetadata, "SIZE");
	if (size == 0) {
		free(rutaMetadata);
		return FAIL;
	}
	t_list* listaBloques = leerClaveValorList(rutaMetadata, "BLOCKS");
	char* archivoMapeado = mapearArchivoEnString(listaBloques);
	char* clave = string_from_format("%d-%d", posX, posY);
	if (!string_contains(archivoMapeado, clave)) {
		free(rutaMetadata);
		free(archivoMapeado);
		free(clave);
		list_destroy_and_destroy_elements(listaBloques, freeElem);
		return FAIL;
	}

	archivoMapeado = operarCatch(archivoMapeado, posX, posY);

	int nuevoTamanio = string_length(archivoMapeado);
	if (nuevoTamanio == 0) {
		//liberar todos los bloques
		for (int a = 0; a < listaBloques->elements_count; a++) {
			liberarBloque(atoi(list_get(listaBloques, a)));
		}
		setClaveValor(rutaMetadata, "BLOCKS", "[]");
		setClaveValor(rutaMetadata, "SIZE", "0");

		free(archivoMapeado);
		free(rutaMetadata);
		list_destroy_and_destroy_elements(listaBloques, freeElem);
		return OK;
	}
	int cantidadBloquesRequeridos = calcularNumeroDeBloquesNecesarios(nuevoTamanio);

	while (listaBloques->elements_count > cantidadBloquesRequeridos) {
		liberarBloque(atoi(list_get(listaBloques, listaBloques->elements_count - 1)));
		list_remove_and_destroy_element(listaBloques, listaBloques->elements_count - 1, freeElem);
	}

	dumpearArchivo(listaBloques, archivoMapeado);

	char* blocksMetadata = stringDesdeLista(listaBloques);
	setClaveValor(rutaMetadata, "BLOCKS", blocksMetadata);
	free(blocksMetadata);

	char* nuevoTamanioAsString = string_itoa(nuevoTamanio);
	setClaveValor(rutaMetadata, "SIZE", nuevoTamanioAsString);
	free(nuevoTamanioAsString);

	free(archivoMapeado);
	free(rutaMetadata);
	free(clave);
	list_destroy_and_destroy_elements(listaBloques, freeElem);
	return OK;

}

//FUNCIONES PARA LECTURA/ESCRITURA DE ARCHIVOS METADATA

int leerClaveValorInt(char* path, char* clave) {
	if (!fileExists(path)) {
		return -1;
	}
	t_config* metadata = config_create(path);
	if (!config_has_property(metadata, clave)) {
		config_destroy(metadata);
	}
	int retorno = config_get_int_value(metadata, clave);
	config_destroy(metadata);
	return retorno;
}

char* leerClaveValorString(char* path, char* clave) {
	if (!fileExists(path)) {
		return NULL;
	}
	t_config* metadata = config_create(path);
	if (!config_has_property(metadata, clave)) {
		config_destroy(metadata);
		return NULL;
	}
	char* retorno = string_new();
	string_append(&retorno, config_get_string_value(metadata, clave));
	config_destroy(metadata);
	return retorno;
}

char** leerClaveValorArray(char* path, char* clave) {
	if (!fileExists(path)) {
		return NULL;
	}
	t_config* metadata = config_create(path);
	if (!config_has_property(metadata, clave)) {
		config_destroy(metadata);
		return NULL;
	}
	char** retorno = config_get_array_value(metadata, clave);

	/* una forma de ver el contenido del char**
	 int a = 0;
	 while (retorno[a] != NULL) {
	 printf("%s\n", retorno[a]); //puts no anda con esto.
	 a++;
	 }
	 if (retorno[0]==NULL) puts("arr vacio");
	 */
	config_destroy(metadata);
	return retorno;
}

void freeArrayChars(char** arr) {
	int a = 0;
	while (arr[a] != NULL) {
		free(arr[a]);
		a++;
	}
	free(arr);
}

t_list* leerClaveValorList(char* path, char* clave) {
	char** arr = leerClaveValorArray(path, clave);
	t_list* lista = list_create();
	int a = 0;
	while (arr[a] != NULL) {
		char* aux = string_duplicate(arr[a]);
		list_add(lista, aux);
		a++;
	}
	freeArrayChars(arr);
	return lista;
}

char* stringDesdeLista(t_list* lista) {
	char* retorno = string_from_format("[");
	for (int a = 0; a < lista->elements_count; a++) {
		if (a == 0) {
			string_append_with_format(&retorno, "%s", list_get(lista, a));
		} else {
			string_append_with_format(&retorno, ",%s", list_get(lista, a));
		}
	}
	string_append(&retorno, "]");
	return retorno;
}

void freeElem(void* elem) {
	free(elem);
}

int setClaveValor(char* path, char* clave, char* valor) {
	if (!fileExists(path)) {
		return -1;
	}
	t_config* metadata = config_create(path);
	if (!config_has_property(metadata, clave)) {
		config_destroy(metadata);
		return -1;
	}
	config_set_value(metadata, clave, valor);
	config_save(metadata);
	config_destroy(metadata);
	return 0;
}

//FUNCIONES DE CHEQUEO DE EXISTENCIA

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

//FUNCIONES DE GENERACION DE RUTAS

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

//FUNCIONES DEL FORMATEADOR

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
