#include "dynamic.h"

static void _consolidate();
static void _compact();
static Partition* find_partition_dynamic(uint32_t);

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
				_compact();
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

	t_list* occupied = get_occupied_partitions();

	for(int i = 0; i < occupied->elements_count; i+2){

		Partition* first_partition = list_get(occupied, i);
		Partition* second_partition = list_get(occupied, i+1);

		if(i == 0){
			first_partition->start = memory->cache;
		} else {
			Partition* third_partition = list_get(occupied, i-1);
			first_partition->start = third_partition->start + third_partition->size;
		}

		second_partition = first_partition->start + first_partition->size;
	}

	t_list* not_occupied = get_free_partitions();

	int free_size = 0;

	int _sum_all_sizes(int accum, Partition* partition){
		return accum += partition->size;
	}

	list_fold(not_occupied, free_size, &_sum_all_sizes);

	Partition* last_ocupied = list_get(occupied, occupied->elements_count -1);

	Partition* new_free_partition = create_partition(last_ocupied->start + last_ocupied->size, free_size);

	void _remove(Partition* partition) {
		remove_partition_at(partition->start);
	}
	list_iterate(not_occupied, &_remove);
	add_partition_next_to(last_ocupied->start, new_free_partition);

	//checks for nulls
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
		if(element->free && element_next->free && element_next != NULL && element != NULL){
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

// 1) filtramos las particiones libres que satisfacen el tamañoAGuardar
// 2) buscamos la particion en base al algoritmo FF / BF
// 3) si encontre una, la rompo si hace falta en 2 particiones (1 del tamanioQueVoyAGuardar)
//  y la otra de lo que le quedo libre
//  Ej: guardo 30, tengo libre una de 80 (nro 7)
//  me queda -> particion a usar (nro 7) tamanio 30 + particion (nro8) tamanio 50
//  // TODO: ver lo del tamanioMinioDeParticion
// 4) devolvemos la particion encontrada o un NULL si no encontre ninguna

// first . filter(free && tamanio>=tamanioABuscar)

static Partition* find_partition_dynamic(uint32_t size_of_data) {

	Partition* partition = find_partition(size_of_data);

	if(partition != NULL) {

		int new_size = MAX_PARTITION_SIZE(size_of_data);

		int old_size = partition->size;

		partition->size = new_size;
		partition->free = false;
		int now = (int) ahoraEnTimeT();
		partition->access_time = now;
		if(size_of_data == old_size) partition->creation_time = now;

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

