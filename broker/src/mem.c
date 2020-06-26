#include "mem.h"

void save_message(void* data, Operation operation, uint32_t message_id, uint32_t correlational_id) {
	Message* message = create_message(operation, message_id, correlational_id, _calculate_data_size(data, operation));
	_save_to_cache(data, message);
}

// TODO: ver si siguen vigentes
void delete_data() {

}
void delete_partition()  {

}

// siempre que mato una particion, tengo que consolidar, siempre
Partition* choose_victim() {
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, FIFO)) {
		return _choose_by_fifo();
	} else {
		return _choose_by_lru();
	}
}

t_list* get_filtered_partitions(uint32_t size_to_compare) {
	bool _greater_equals_and_free(Partition* partition) {
		return greater_equals_and_free(size_to_compare, partition);
	}
	return list_filter(memory->partitions, &_greater_equals_and_free);
}

// Test: si no lo terminamos usando borrar
t_list* get_occupied_partitions() {
	return list_filter(memory->partitions, &_is_occupied);
}

// Test: si no lo terminamos usando borrar
t_list* get_sorted_partitions() {
	t_list* partitions = list_filter(memory->partitions, &_is_occupied);
	list_sort(partitions, &_less_access_time);
	return partitions;
}

// Test: si no lo terminamos usando borrar
Partition* test_lru(t_list* partitions) {
	t_list* occupied_partitions = list_filter(partitions, &_is_occupied);

	if(occupied_partitions->elements_count > 0) {
		list_sort(occupied_partitions, &_less_access_time);
		return list_get(occupied_partitions, 0);
	}
	return NULL;
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
		save_to_cache_dynamic(data, message);
	}
}


static bool greater_equals_and_free(uint32_t to_compare, Partition* partition) {
	return partition->free && partition->size >= to_compare;
}

// TODO: cambiar por la posta
// ver como trackear a la siguiente victimas
static Partition* _choose_by_fifo() {
	return list_get(memory->partitions, 0);
}

static Partition* _choose_by_lru() {
	t_list* occupied_partitions = list_sorted(list_filter(memory->partitions, &_is_occupied), &_less_access_time);

	if(occupied_partitions->elements_count > 0) {
		return list_get(occupied_partitions, 0);
	}
	return NULL;
}

static bool _less_access_time(Partition* partition_a, Partition* partition_b) {
	return partition_a->access_time < partition_b->access_time;
}

static bool _is_occupied(Partition* partition) {
	return !partition->free;
}
