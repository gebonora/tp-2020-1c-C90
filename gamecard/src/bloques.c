/*
 * bloques.c
 *
 *  Created on: 18 jun. 2020
 *      Author: utnso
 */
#include "bloques.h"

void crearBloques() {
	log_info(loggerMain, "----Creando '%d' bloques de '%d'...", g_numberOfBlocks, g_blockSize);
	for (int a = 0; a < g_numberOfBlocks; a++) {
		char* rutaBloque = crearRutaBloque(a);
		FILE * fp = fopen(rutaBloque, "w+");
		fclose(fp);
		truncate(rutaBloque, g_blockSize);
		free(rutaBloque);
	}
	log_info(loggerMain, "----Listo!");
}

int asignarBloqueLibre() { // RETORNA -1 SI NO HAY BLOQUES LIBRES.
	int bloqueLibre = -1;
	pthread_mutex_lock(&m_bitmap);
	for(int a = 0; a< g_numberOfBlocks; a++){
		if(!bitarray_test_bit(g_bitmap,a)){
			bitarray_set_bit(g_bitmap,a);
			bloqueLibre = a;
			break;
		}
	}
	pthread_mutex_unlock(&m_bitmap);
	return bloqueLibre;
}

void liberarBloque(int numeroDeBloque) {
	pthread_mutex_lock(&m_bitmap);
	bitarray_clean_bit(g_bitmap, numeroDeBloque);
	pthread_mutex_unlock(&m_bitmap);
}

char* crearRutaBloque(int numeroDeBloque) {
	char* pathDirectorio = crearRuta("Blocks");
	char* rutaBloque = string_new();
	string_append_with_format(&rutaBloque, "%s/%d.bin", pathDirectorio, numeroDeBloque);
	free(pathDirectorio);
	return rutaBloque;
}
