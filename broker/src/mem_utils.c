#include "mem_utils.h"

static Partition* _choose_victim(bool(*)(void*));
static bool _is_occupied(Partition*);
static bool _less_access_time(Partition*, Partition*);
static bool _less_creation_time(Partition*, Partition*);
static t_list* _get_occupied_partitions();
static void _show_partition(Partition*);
static void _show_message(Message*);
static bool _greater_equals_and_free(uint32_t, Partition*);
static t_link_element* _find_partition(int, bool);
static t_list* greater_equals_and_free(uint32_t);
static bool _smaller_size(Partition*, Partition*);
static t_link_element* _list_get_element(t_list*, int);

/** PUBLIC FUNCTIONS **/

t_link_element* find_partition(int size_to_fit) {
	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE, FIRST_FIT)) {
		return _find_partition(size_to_fit, false);
	} else {
		return _find_partition(size_to_fit, true);
	}
}

Partition* choose_victim() {
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, FIFO)) {
		return _choose_victim(&_less_creation_time);
	} else {
		return _choose_victim(&_less_access_time);
	}
}

Partition* create_partition(uint32_t partition_number, uint32_t partition_size, uint32_t* partition_start, uint32_t position, Message* message) {
	Partition* partition = malloc(sizeof(Partition));
	partition->access_time = (int) ahoraEnTimeT();
	partition->free = partition_number % 2 == 0 ? true : false;
	partition->number = partition_number;
	partition->size = partition_size;
	partition->start = partition_start;
	partition->position = position;
	partition->message = message;
	partition->creation_time = (int) ahoraEnTimeT();
	return partition;
}

void show_partitions(t_list* partitions) {
	log_info(LOGGER, "--------------------------------");
	log_info(LOGGER, "Partitions size: %d", memory->partitions->elements_count);
	list_iterate(partitions, &_show_partition);
}

void show_memory_partitions() {
	log_info(LOGGER, "--------------------------------");
	log_info(LOGGER, "Partitions size: %d", memory->partitions->elements_count);
	list_iterate(memory->partitions, &_show_partition);
}

/** PRIVATE FUNCTIONS **/

static t_link_element* _find_partition(int desired_size, bool best) {
	t_list* potential_partitions = greater_equals_and_free(desired_size);
	if(best) list_sort(potential_partitions, &_smaller_size);
	return _list_get_element(potential_partitions, 0);
}

static t_link_element* _list_get_element(t_list* self, int index) {
	int cont = 0;

	if ((self->elements_count > index) && (index >= 0)) {
		t_link_element *element = self->head;
		while (cont < index) {
			element = element->next;
			cont++;
		}
		return element;
	}
	return NULL;
}

static t_list* greater_equals_and_free(uint32_t size_to_compare) {
	bool _inline_greater_equals_and_free(Partition* partition) {
		return _greater_equals_and_free(size_to_compare, partition);
	}
	return list_filter(memory->partitions, &_inline_greater_equals_and_free);
}

static bool _smaller_size(Partition* partition_1, Partition* partition_2) {
	return partition_1->size < partition_2->size;
}

static void _show_partition(Partition* partition) {
	log_info(LOGGER, "Partition #%d", partition->number);
	log_info(LOGGER, "Free: %s", partition->free ? "true" : "false");
	log_info(LOGGER, "Start: %d - %x", partition->position, partition->start);
	log_info(LOGGER, "Size: %d", partition->size);
	log_info(LOGGER, "Buddy: %d - %x", xor_int_and_int(partition->position, partition->size), xor_pointer_and_int(partition->start, partition->size));
	log_info(LOGGER, "Last access: %d", partition->access_time);
	_show_message(partition->message);
	log_info(LOGGER, "--------------------------------");
}

static void _show_message(Message* message) {
	log_info(LOGGER, "Message Queue: %s", get_operation_by_value(message->operation_code));
	log_info(LOGGER, "Message ID: %d", message->message_id);
	log_info(LOGGER, "Correlative ID: %d", message->correlational_id);
	log_info(LOGGER, "Message Size: %d", message->data_size);
}

static bool _greater_equals_and_free(uint32_t to_compare, Partition* partition) {
	return partition->free && partition->size >= to_compare;
}

static Partition* _choose_victim(bool(*condition)(void*)) {
	t_list* occupied_partitions = list_sorted(_get_occupied_partitions(memory->partitions), &condition);
	return list_get(occupied_partitions, 0);
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
