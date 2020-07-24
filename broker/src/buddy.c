#include "buddy.h"

static void _consolidate_buddy(Partition*);
static Partition* _break_buddys(Partition*, uint32_t);
static Partition* _buddy_of(Partition*);
static Partition* _find_partition_at_position(uint32_t);
static bool _partition_at_position(uint32_t, uint32_t);
static uint32_t _buddy_position(Partition*);

// 1) calculo la potencia de 2 mas cercana (hacia arriba) al valor de tamanioDeMiDato
//  2) busco una particion libre de tamanio = pot2
//   	   2.a) si la encuentro guardo el dato
//   	   2.b.i) si no la encuentro, busco la particion libre mas cercana a la pot2 calculada
//   	   2.b.ii) genero n buddys particionando hasta que me queda el tamanio del buddy = pot2 calculadass
//   3) si no encontre ninguna particion, elijo una victima y vuelvo a lanzar la busqueda
Partition* save_to_cache_buddy_system(void* data, Message* message) {
	sem_wait(&MEMORY);
	log_debug(LOGGER, "MAX between MIN_PARTITION_SIZE (%d) and next_power_of_2 of data_size (%d)", TAMANO_MINIMO_PARTICION, message->data_size);
	int desired_size = MAX_PARTITION_SIZE(next_power_of_2(message->data_size));

	log_debug(LOGGER, "Desired size: %d", desired_size);
	log_debug(LOGGER, "Finding free partition");
	// busco una particion libre
	Partition* partition = find_partition(desired_size);

	// busco una particion libre y sino elimino alguna ocupada y consolido
	while(partition == NULL) {
		log_debug(LOGGER, "Free partition not found");
		log_info(LOGGER, "Choosing victim");
		Partition* victim = choose_victim();
		log_debug(LOGGER, "Consolidating buddy");
		_consolidate_buddy(victim);
		log_debug(LOGGER, "Trying to find free partition again");
		partition = find_partition(desired_size);
	}

	log_debug(LOGGER, "Free partition found. Check if needs to be broken");
	if(partition->size != desired_size) {
		// rompo la particion libre elegida, hasta la minima pot2 posible
		Partition* choosed_partition = _break_buddys(partition, desired_size);
	}

	// marco la particion como ocupada, y completo el resto de los atributos
	partition->message = message;
	uint32_t now = get_time();
	partition->creation_time = now;
	partition->access_time = now;
	partition->free = false;


	log_debug(LOGGER, "Partition broken, doing memcpy in start=%x, with data_size=%d", partition->start, message->data_size);
	// guardo el data con el memcpy
	memcpy(partition->start, data, message->data_size);
	log_info(LOGGER, "Mensaje %d guardado con exito en la particion que comienza en %d",partition->message->message_id, partition->position);

	log_debug(LOGGER, "Done memcpy");
	sem_post(&MEMORY);
	free(data);
	return partition;

}

/** PRIVATE FUNCTIONS **/

static void _consolidate_buddy(Partition* partition) {
	Partition* buddy = _buddy_of(partition);

	// si puedo consolidar, arranco, sino no hago nada
	while(buddy->free && buddy->size == partition->size) {
		log_info(LOGGER, "Partition Position: %d, Size: %d, Free: %s", partition->position, partition->size, partition->free ? "true" : "false");
		log_info(LOGGER, "Buddy Position: %d, Size: %d, Free: %s", buddy->position, buddy->size, buddy->free ? "true" : "false");

		log_info(LOGGER, "Buddy is free and same size...consolidating");

		log_debug(LOGGER, "Merging partition with buddy in new partition");
		// cambio la particion, sumando los tamanios y utilizando los minimos start y position
		partition->position = MIN(partition->position, buddy->position);
		partition->start = MIN(partition->start, buddy->start);
		partition->size = SUM(partition->size, buddy->size);

		log_debug(LOGGER, "Removing buddy because it was absorbed");
		// elimino el buddy que sobro, ya que fue absorbido
		remove_partition_at(buddy->start);

		log_debug(LOGGER, "Calculating buddy of new partition (replacement)");
		// obtengo el buddy de la nueva particion consolidada
		buddy = _buddy_of(partition);

		log_debug(LOGGER, "Partition Position: %d, Size: %d, Free: %s", partition->position, partition->size, partition->free ? "true" : "false");
		log_debug(LOGGER, "Buddy Position: %d, Size: %d, Free: %s", buddy->position, buddy->size, buddy->free ? "true" : "false");
	}

	log_info(LOGGER, "Buddy is not free or same size...done consolidating");
}

static Partition* _break_buddys(Partition* partition_to_break, uint32_t data_size) {
	// mientras la particion actual sea mayor a mi data_size (que ya es pot2), entonces sigo rompiendo
	log_debug(LOGGER, "Partition size: %d, Data size: %d", partition_to_break->size, data_size);

	while(partition_to_break->size != data_size){

		log_debug(LOGGER, "Partition's size is different from desired, breaking it in two of same size");

		// izq -> size = a la que rompo dividido 2, start = start de la que rompo
		partition_to_break->size = HALF(partition_to_break->size);
		log_debug(LOGGER, "Left partition - Position %d, Start: %x, Size: %d", partition_to_break->position, partition_to_break->start, partition_to_break->size);

		// der -> size = al de la iz, start = start izq + size
		Partition* right_partition = create_partition(SUM(partition_to_break->position, partition_to_break->size), SUM(partition_to_break->start, partition_to_break->size), partition_to_break->size);
		log_debug(LOGGER, "Right partition - Position %d, Start: %x, Size: %d", right_partition->position, right_partition->start, right_partition->size);

		log_debug(LOGGER, "Adding right partition next to left one");
		// agrego la de la derecha al lado de la de la izq
		add_partition_next_to(partition_to_break->start, right_partition);

		log_debug(LOGGER, "Partition size: %d, Data size: %d", partition_to_break->size, data_size);
	}

	log_debug(LOGGER, "Partition's size is same as desired, no breaking needed");
	// cuando ya no tengo que romper mas, devuelvo la que me quedo, que es del tamanio deseado
	return partition_to_break;
}

static Partition* _buddy_of(Partition* partition) {
	return _find_partition_at_position(_buddy_position(partition));
}

static uint32_t _buddy_position(Partition* partition) {
	return xor_int_and_int(partition->position, partition->size);
}

static Partition* _find_partition_at_position(uint32_t position_to_find) {
	bool _inline_partition_at_position(Partition* partition) {
		return _partition_at_position(position_to_find, partition->position);
	}

	return list_find(memory->partitions, &_inline_partition_at_position);
}

static bool _partition_at_position(uint32_t position_to_compare, uint32_t actual_position) {
	return position_to_compare == actual_position;
}
