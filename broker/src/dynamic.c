#include "dynamic.h"

static void _consolidate();
static void _compact();
static Partition* find_partition_dynamic(uint32_t);
static Partition* _negative_frecuency(uint32_t);
static Partition* _positive_frecuency(uint32_t);
static Partition* _zero_frecuency(uint32_t);

Partition* save_to_cache_dynamic_partitions(void* data, Message* message){
	sem_wait(&MEMORY);
	Partition* partition;

	log_debug(LOGGER, "Frecuencia Compactacion: %d", FRECUENCIA_COMPACTACION);

	if(FRECUENCIA_COMPACTACION < 0 ){
		partition = _negative_frecuency(message->data_size);
	} else if(FRECUENCIA_COMPACTACION == 0) {
		partition = _zero_frecuency(message->data_size);
	} else {
		partition = _positive_frecuency(message->data_size);
	}

	log_debug(LOGGER, "Saving message to partition");
	partition->message = message;
	uint32_t now = get_time();
	partition->access_time = now;
	partition->creation_time = now;
	log_debug(LOGGER, "Copying bytes to cache");
	memcpy(partition->start, data, message->data_size);
	log_info(LOGGER, "Mensaje %d guardado con exito en la particion que comienza en %d",partition->message->message_id, partition->position);
	log_debug(LOGGER, "memcpy done");
	sem_post(&MEMORY);
	free(data);
	return partition;
}

/*
 * Tengo toda las particiones, debo agarrar las libres y hacer una sola con todos los tamaños.
 * Esa la tengo que agregar al final de la lista.
 *
 * */

static Partition* _negative_frecuency(uint32_t data_size){
	log_debug(LOGGER, "Starting negative frecuency routine");

	Partition* partition = NULL;

	while(partition == NULL){
		log_debug(LOGGER, "Finding free partition");
		partition = find_partition_dynamic(data_size);
		if (partition == NULL) {
			log_debug(LOGGER, "Free partition not found. Choosing victim");
			choose_victim();//que tambien la mata
			log_debug(LOGGER, "Consolidating victim");
			_consolidate();
		}
	}

	return partition;
}

static Partition* _zero_frecuency(uint32_t data_size){
	log_debug(LOGGER, "Starting zero frecuency routine");

	Partition* partition = NULL;
	bool kill = false;

	while(partition == NULL){
		log_debug(LOGGER, "Finding free partition");
		partition = find_partition_dynamic(data_size);
		if (partition == NULL) {
			log_debug(LOGGER, "Free partition not found");
			log_debug(LOGGER, "Kill status: %s", kill ? "true" : "false");
			if(kill){
				log_info(LOGGER, "Choosing victim");
				choose_victim();//que tambien la mata
				log_debug(LOGGER, "Consolidating victim");
				_consolidate();
				kill = false;
			} else {
				kill = true;
			}
			log_info(LOGGER, "Compacting");
			_compact();
		}
	}

	return partition;
}

static Partition* _positive_frecuency(uint32_t data_size) {
	log_debug(LOGGER, "Starting positive frecuency routine");

	int partitions_killed = 0;
	Partition* partition = NULL;

	while(partition == NULL){
		log_debug(LOGGER, "Finding free partition");
		partition = find_partition_dynamic(data_size);

		if(partition == NULL){
			log_debug(LOGGER, "Free partition not found");
			log_debug(LOGGER, "Frequency (%d) - Partitions killed (%d)", FRECUENCIA_COMPACTACION, partitions_killed);
			if(FRECUENCIA_COMPACTACION == partitions_killed) {
				log_debug(LOGGER, "Frequency is equal to partitions killed. Compacting");
				_compact();
				partitions_killed = 0;
			} else {
				log_debug(LOGGER, "Frequency is different to partitions killed. Choosing victim");
				choose_victim();//que tambien la mata
				log_debug(LOGGER, "Consolidating victim");
				_consolidate();
				partitions_killed++;
			}
		}
	}

	return partition;
}

static void _compact() {

	log_debug(LOGGER, "Getting ocuppied partitions");
	t_list* occupied = get_occupied_partitions();
	uintptr_t start = memory->cache;
	uint32_t position = 0;

	for(int index = 0; index < occupied->elements_count; index++) {
		Partition* partition = list_get(occupied, index);

		log_debug(LOGGER, "Modifying partition (position=%d, start=%x, size=%d", partition->position, partition->start, partition->size);

		log_debug(LOGGER, "New values for start=%x, position=%d", start, position);
		partition->start = start;
		partition->position = position;

		start = SUM(start, partition->size);
		position = SUM(position, partition->size);
	}

	log_debug(LOGGER, "Getting free partitions");
	t_list* not_occupied = get_free_partitions();

	log_debug(LOGGER, "Free partitions count: %d", not_occupied->elements_count);

	uint32_t _sum_all_sizes(uint32_t accum, Partition* partition){
		log_debug(LOGGER, "Accumulator (%d) += %d", accum, partition->size);
		return accum + partition->size;
	}

	log_debug(LOGGER, "Summing free sizes");
	uint32_t free_size = list_fold(not_occupied, 0, &_sum_all_sizes);

	log_debug(LOGGER, "Total free size: %d", free_size);

	log_debug(LOGGER, "Getting last occupied partition");
	Partition* last_occupied = list_get(occupied, occupied->elements_count -1);

	log_debug(LOGGER, "Last occupied partition (position=%d, start=%x, size=%d)", last_occupied->position, last_occupied->start, last_occupied->size);

	log_debug(LOGGER, "Creating free big partition");

	Partition* new_free_partition = create_partition(last_occupied->position + last_occupied->size, last_occupied->start + last_occupied->size, free_size);

	log_debug(LOGGER, "Free big partition (position=%d, start=%x, size=%d)", new_free_partition->position, new_free_partition->start, new_free_partition->size);

	void _remove(Partition* partition) {
		remove_partition_at(partition->start);
	}

	log_debug(LOGGER, "Removing all free partitions from memory");
	list_iterate(not_occupied, &_remove);

	log_debug(LOGGER, "Adding new free big partition at the end");
	add_partition_next_to(last_occupied->start, new_free_partition);
	list_destroy(occupied);
	list_destroy(not_occupied);

	log_info(LOGGER, "Compact done");
}


/** PRIVATE FUNCTIONS **/

/*Busco la primera libre.
 * Me fijo si la siguiente esta libre. Si es asi, la absorbo.
 * Por las dudas tengo que fijarme si la siguiente tambien esta libre (caso en el que borro una particion que esta rodeada de dos libres).
 * Si es asi, la absorbo.
 */
static void _consolidate(){

	Partition* left_partition = NULL;
	Partition* middle_partition = NULL;
	Partition* right_partition = NULL;

	log_debug(LOGGER, "Iterating partitions to find free and adjacent");
	log_debug(LOGGER, "Memory partitions count: %d", memory->partitions->elements_count);
	for(int i = 0; i < memory->partitions->elements_count; i++) {
		Partition* element = list_get(memory->partitions, i);
		Partition* element_next = list_get(memory->partitions, i+1);
		Partition* element_next2 = list_get(memory->partitions, i+2);
		log_debug(LOGGER, "Evaluating element (position=%d, size=%d, free=%s)", element->position, element->size, element->free ? "true": "false");
		if(element != NULL && element->free && element_next != NULL && element_next->free) {
			log_debug(LOGGER, "Element (position=%d) is free and has a free adjacent (position=%d)", element->position, element_next->position);

			left_partition = element;
			middle_partition = element_next;

			if(element_next2 != NULL && element_next2->free){
				log_debug(LOGGER, "Adjacent (position=%d) has a free adjacent (position=%d)", element_next->position, element_next2->position);
				right_partition = element_next2;
			}

			break;
		}
	}

	if(left_partition != NULL){
		log_info(LOGGER, "Starting consolidation");
		log_info(LOGGER, "First free partition (position=%d, start=%x, size=%d)", left_partition->position, left_partition->start, left_partition->size);
		log_info(LOGGER, "Second free partition (position=%d, start=%x, size=%d)", middle_partition->position, middle_partition->start, middle_partition->size);

		left_partition->size = left_partition->size + middle_partition->size;

		if(right_partition != NULL){
			log_info(LOGGER, "Third free partition (position=%d, start=%x, size=%d)", right_partition->position, right_partition->start, right_partition->size);

			left_partition->size = left_partition->size + right_partition->size;

			log_debug(LOGGER, "Removing third partition");
			remove_partition_at(right_partition->start);
		}

		log_debug(LOGGER, "Removing second partition");
		remove_partition_at(middle_partition->start);

		log_info(LOGGER, "Consolidated partition (position=%d, start=%x, size=%d)", left_partition->position, left_partition->start, left_partition->size);
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

		log_debug(LOGGER, "Free partition found (position=%d, start=%x, size=%d)", partition->position, partition->start, partition->size);

		log_debug(LOGGER, "Calculating new size as MAX(size_of_data=%d, TAMANO_MINIMO_PARTICION=%d)", size_of_data, TAMANO_MINIMO_PARTICION);
		int new_size = MAX_PARTITION_SIZE(size_of_data);

		int old_size = partition->size;

		log_debug(LOGGER, "Partition new size: %d, old size: %d", new_size, old_size);
		uint32_t now = get_time();

		if(old_size != new_size){
			log_debug(LOGGER, "New size is different than old_size. Breaking partition");

			partition->size = new_size;
			partition->free = false;

			log_debug(LOGGER, "Updated partition (position=%d, start=%x, size=%d)", partition->position, partition->start, partition->size);

			Partition* new_partition = create_partition(partition->position + new_size, partition->start + new_size, old_size - new_size);

			log_debug(LOGGER, "New partition created (position=%d, start=%x, size=%d)", new_partition->position, new_partition->start, new_partition->size);

			log_debug(LOGGER, "Adding new partition next to broken partition");
			add_partition_next_to(partition->start, new_partition);
		}

		log_debug(LOGGER, "Updating partition attributes (free, creation_time, access_time)");
		partition->free = false;

		// tamanio particion encontrada >  tamanioAGuardar -> trunco particion encontrada y genero una nueva con el excedente
		return partition;
	} else {
		return NULL;
	}

}

