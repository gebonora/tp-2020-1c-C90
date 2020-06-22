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

}

void agregarSemaforoALista(char* nombreArchivo){
	//agregar a la lista una entrada con el nombre y el semaforo, inicializandolo.
}

int accederAArchivo(char* nombreArchivo){//por fuera de esta funcion tenemos un while con el sleep de reintento operacion
	int resultado = -1;
	//espera por el mutex del archivo.

	//consulta el OPEN. si está en Y sale. return -1
	//					si esá en N lo graba como Y y entra. return 0

	//libera el semaforo
	return resultado;
}
