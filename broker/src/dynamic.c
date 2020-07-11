#include "dynamic.h"

static void _consolidate();
static void _compact();

// 1) filtramos las particiones libres que satisfacen el tamañoAGuardar
// 2) buscamos la particion en base al algoritmo FF / BF
// 3) si encontre una, la rompo si hace falta en 2 particiones (1 del tamanioQueVoyAGuardar)
//  y la otra de lo que le quedo libre
//  Ej: guardo 30, tengo libre una de 80 (nro 7)
//  me queda -> particion a usar (nro 7) tamanio 30 + particion (nro8) tamanio 50
//  // TODO: ver lo del tamanioMinioDeParticion
// 4) devolvemos la particion encontrada o un NULL si no encontre ninguna

// first . filter(free && tamanio>=tamanioABuscar)

Partition* find_partition_dynamic(uint32_t sizeOfData) {

	Partition* partition = find_partition(sizeOfData);

	if(partition != NULL) {

		int new_size = MAX_PARTITION_SIZE(sizeOfData);

		int old_size = partition->size;

		partition->size = new_size;
		partition->free = false;
		int now = (int) ahoraEnTimeT();
		partition->access_time = now;
		partition->creation_time = now;
		// TODO: ver access time,
		 // ver metodo de seba nueva particion

		replace_partition_at(partition->start, partition);

		Partition* new_partition = create_partition(partition->start + new_size, old_size - new_size);

		add_partition_next_to(partition->start, new_partition);

		// ROMPER LA PARTICION
		// tamanio particion encontrada == tamanioAGuardar -> devuelvo asi
		// tamanio particion encontrada >  tamanioAGuardar -> trunco particion encontrada y genero una nueva con el excedente
		return partition;
	} else {
		return NULL;
	}
}


void save_to_cache_dynamic_partitions(void* data, Message* message) {
	int partitions_killed = 0;
	Partition* partition = NULL;

	while(partition == NULL){
		partition = find_partition_dynamic(message->data_size);
		if (partition != NULL) {
			partition->message = message;
			memcpy(partition->start, data, message->data_size);
		} else {
			if(FRECUENCIA_COMPACTACION == partitions_killed) {//casos igual o 0
				compactar();
			} else {
				choose_victim();//que tambien la mata
				_consolidate();
			}
			partitions_killed++;
		}
	}
}

/*
 * Tengo toda las particiones, debo agarrar las libres y hacer una sola con todos los tamaños.
 * Esa la tengo que agregar al final de la lista.
 *
 * */
static void _compact() {

}


/** PRIVATE FUNCTIONS **/

/*Busco la primera libre.
 * Me fijo si la siguiente esta libre. Si es asi, la absorbo.
 * Por las dudas tengo que fijarme si la siguiente tambien esta libre (caso en el que borro una particion que esta rodeada de dos libres).
 * Si es asi, la absorbo.
 */
static void _consolidate(){

	Partition* left_partition;
	Partition* middle_partition;
	Partition* right_partition;

	for(int i = 0; i < memory->partitions->elements_count; i++){
		Partition* element = list_get(memory->partitions, i);
		Partition* element_next = list_get(memory->partitions, i+1);
		Partition* element_next2 = list_get(memory->partitions, i+2);
		if(element->free && element_next->free && element_next != NULL){
			left_partition = element;
			middle_partition = element_next;

			if(element_next2->free && element_next2 != NULL){
				right_partition = element_next2;
			}
			break;
		}
	}

	if(left_partition != NULL){

		left_partition->size = left_partition->size + middle_partition->size;

		if(right_partition != NULL){

			left_partition->size = left_partition->size + right_partition->size;

			remove_partition_at(right_partition->start);
		}

		remove_partition_at(middle_partition->start);
	}
}
