#include "mem.h"

static int _calculate_data_size(void*, Operation);
static void _save_to_cache(void*, Message*);
static Partition* _choose_victim(bool(*condition)(void*));
static bool _is_occupied(Partition*);
static bool _less_access_time(Partition*, Partition*);
static bool _less_creation_time(Partition*, Partition*);
static t_list* _get_occupied_partitions();

/** PUBLIC FUNCTIONS **/

void save_message(void* data, Operation operation, uint32_t message_id, uint32_t correlational_id) {
	Message* message = create_message(operation, message_id, correlational_id, _calculate_data_size(data, operation));
	_save_to_cache(data, message);
}

// TODO: ver si sigue vigente
void delete_data() {

}
// TODO: ver si sigue vigente
void delete_partition()  {

}

// siempre que mato una particion, tengo que consolidar, siempre
Partition* choose_victim() {
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, FIFO)) {
		return _choose_victim(&_less_creation_time);
	} else {
		return _choose_victim(&_less_access_time);
	}
}


/** PRIVATE FUNCTIONS **/


static int _calculate_data_size(void* data, Operation operation) {
	int size = 0;

	switch (operation) {
	case NEW: ;
		size = calculate_new_bytes(data);
		break;
	case APPEARED:
	case CATCH: ;
		size = calculate_pokemon_bytes(data);
		break;
	case GET: ;
		size = calculate_get_bytes(data);
		break;
	case LOCALIZED: ;
		size = calculate_localized_bytes(data);
		break;
	case CAUGHT: ;
		size = calculate_caught_bytes();
		break;
	}
	return size;
}

static void _save_to_cache(void* data, Message* message) {
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		save_to_cache_buddy_system(data, message);
	} else {
		save_to_cache_dynamic_partitions(data, message);
	}
}

static Partition* _choose_victim(bool(*condition)(void*)) {
	t_list* occupied_partitions = list_sorted(_get_occupied_partitions(memory->partitions), &condition);
	Partition* partition_victim = list_get(occupied_partitions, 0);
	partition_victim->free = true;
	return partition_victim;
}


static bool _is_occupied(Partition* partition) {
	return !partition->free;
}

static bool _less_access_time(Partition* partition_a, Partition* partition_b) {
	return partition_a->access_time < partition_b->access_time;
}

static bool _less_creation_time(Partition* partition_a, Partition* partition_b) {
	return partition_a->creation_time < partition_b->creation_time;
}

static t_list* _get_occupied_partitions() {
	return list_filter(memory->partitions, &_is_occupied);
}
