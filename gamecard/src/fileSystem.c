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

int main(){
	puts("arranco la wea");
	cargarConfigConexiones();
	iniciarLogger();
	crearDirectoriosBase("Metadata");
	crearDirectoriosBase("Files");
	crearArchivosBase("Metadata/Metadata.bin");
	crearArchivosBase("Metadata/Bitmap.bin");
	return 0;
}

// Al FS hay que proveerle un punto de montaje valido, a partir de ahi él crea toddo lo que necesita

void crearDirectoriosBase(char* subPath){
	char* path = string_new();
	string_append_with_format(&path, "%s/%s", PUNTO_MONTAJE_TALLGRASS, subPath);
	DIR* directorio = opendir(path);
	if (directorio == NULL){
		puts(path);
		mkdir(path,0777);
	}
}

void crearArchivosBase(char* subPath){
	char* path = string_new();
	string_append_with_format(&path, "%s/%s", PUNTO_MONTAJE_TALLGRASS, subPath);
	if(fopen(path,"r")==NULL){
		fopen(path,"w");
	}
}



