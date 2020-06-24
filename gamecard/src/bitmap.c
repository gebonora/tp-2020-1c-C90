/*
 * bitmap.c
 *
 *  Created on: 18 jun. 2020
 *      Author: GONZALO BONORA
 */

#include "bitmap.h"

//PARA PERSISTIR EN BITMAP USAREMOS MMAP.

void crearBitmap() {
	crearArchivosBase(PATH_ARCHIVO_BITMAP);
	char* path = crearRuta(PATH_ARCHIVO_BITMAP);
	log_debug(loggerMain, "----Creando Bitmap...");

	int bitarraySize = calcularTamanioBitArray(g_numberOfBlocks);
	log_debug(loggerMain, "----Tamanio del bitarray: '%d'.", bitarraySize);
	char* aux = malloc(bitarraySize);
	t_bitarray* bitmap = bitarray_create_with_mode(aux, bitarraySize, MSB_FIRST);

	truncate(path, bitarraySize);
	int bitmapFD = open(path, O_RDWR, 0);

	char* mapeo = mmap(0, bitarraySize, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, bitmapFD, 0);
	if (mapeo == MAP_FAILED)
		perror("mmap:");

	bitmap->bitarray = mapeo;

	for (int a = 0; a < g_numberOfBlocks; a++) {
		bitarray_clean_bit(bitmap, a);
	}
	munmap(mapeo, bitarraySize);
	close(bitmapFD);
	bitarray_destroy(bitmap);
	free(path);
	free(aux);
	log_debug(loggerMain, "----Listo!");
}

void cargarBitmap() {
	char* path = crearRuta(PATH_ARCHIVO_BITMAP);

	l_bitmapFD = open(path, O_RDWR, 0);

	int bitarraySize = calcularTamanioBitArray(g_numberOfBlocks);

	l_mapeo = mmap(0, bitarraySize, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, l_bitmapFD, 0);
	if (l_mapeo == MAP_FAILED)
		perror("mmap:");

	char* aux = malloc(bitarraySize);
	g_bitmap = bitarray_create_with_mode(aux, bitarraySize, MSB_FIRST);

	g_bitmap->bitarray = l_mapeo;

	free(path);
	free(aux);
	//Dejamos g_bitmap, l_mapeo y l_bitmapFD sin cerrar porque los usaremos hasta que termine el programa.
}

void dumpBitmap(t_bitarray* bitmap, int cantidadMostrada) {
	puts("\n");
	for (int a = 0; a < cantidadMostrada; a++) {
		if (bitarray_test_bit(g_bitmap, a))
			printf("		Bloque %d, valor 1\n", a);
		else
			printf("		Bloque %d, valor 0\n", a);
	}
}

int calcularTamanioBitArray(int bloques) { //Calcula el menor tamaño posible para que ocupen los bits del bitarray.
	if (!bloques % 8) {                    //El malloc va a ocupar este numero*8bits, porque es un malloc de enteros.
		return bloques / 8;
	} else {
		return (bloques / 8) + 1;
	}
}

void cerrarBitmap() {
	int size = calcularTamanioBitArray(g_numberOfBlocks);
	munmap(l_mapeo, size);
	close(l_bitmapFD);
	bitarray_destroy(g_bitmap);
}

