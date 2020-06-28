#include "mem_utils.h"

static Partition* _choose_victim(bool(*)(void*));
static bool _is_occupied(Partition*);
static bool _less_access_time(Partition*, Partition*);
static bool _less_creation_time(Partition*, Partition*);
static t_list* _get_occupied_partitions();
static void _show_partitions_with_index(t_list*);
static void _show_partition(Partition*, int, int);
static void _show_message(Message*);
static bool _greater_equals_and_free(uint32_t, Partition*);
static t_link_element* _find_partition(int, bool);
static t_list* greater_equals_and_free(uint32_t);
static bool _smaller_size(Partition*, Partition*);
static t_link_element* _list_get_element(t_list*, int);
static void _free_partition(Partition*);
static int _list_find_index(uintptr_t);
static bool _partition_at_position(uintptr_t, uintptr_t);

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

Partition* create_partition(uintptr_t start, uint32_t size) {
	int now = (int) ahoraEnTimeT();
	Partition* partition = malloc(sizeof(Partition));
	partition->creation_time = now;
	partition->access_time = now;
	partition->free = true;
	partition->size = size;
	partition->start = start;
	return partition;
}

Partition* create_partition_with_message(uintptr_t start, uint32_t size, Message* message) {
	Partition* partition = create_partition(size, start);
	partition->message = message;
	return partition;
}

void replace_partition_at(uintptr_t position_to_replace, Partition* replacement) {
	list_replace_and_destroy_element(memory->partitions, _list_find_index(position_to_replace), replacement, &_free_partition);
}

void remove_partition_at(uintptr_t position_to_delete) {
	list_remove_and_destroy_element(memory->partitions, _list_find_index(position_to_delete), &_free_partition);
}

void add_partition_next_to(uintptr_t position, Partition* partition_to_add) {
	list_add_in_index(memory->partitions, NEXT(_list_find_index(position)), partition_to_add);
}

Partition* find_partition_at(uintptr_t position_to_find) {
	bool _inline_partition_at_position(Partition* partition) {
		return _partition_at_position(position_to_find, partition->start);
	}

	return list_find(memory->partitions, &_inline_partition_at_position);
}

void show_partitions(t_list* partitions) {
	_show_partitions_with_index(partitions);
}

void show_memory_partitions() {
	_show_partitions_with_index(memory->partitions);
}

/** PRIVATE FUNCTIONS **/

static void _show_partitions_with_index(t_list* list) {
	log_info(LOGGER, "--------Partitions Size: %d---------", list->elements_count);

	t_link_element *element = list->head;
	t_link_element *aux = NULL;
	Partition* partition;

	int index;
	int position = 0;

	while (element != NULL) {
		aux = element->next;
		partition = element->data;
		_show_partition(partition, index, position);
		element = aux;
		position += partition->size;
		index++;
	}
}

static t_link_element* _find_partition(int desired_size, bool best) {
	t_list* potential_partitions = greater_equals_and_free(desired_size);
	if(best) list_sort(potential_partitions, &_smaller_size);
	return _list_get_element(potential_partitions, 0);
}

static int _list_find_index(uintptr_t position_to_find) {
	t_link_element *element = memory->partitions->head;
	int position = 0;

	bool _inline_partition_at_position(Partition* partition) {
		return _partition_at_position(position_to_find, partition->start);
	}

	while (element != NULL && !_inline_partition_at_position(element->data)) {
		element = element->next;
		position++;
	}

	return position;
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

static void _show_partition(Partition* partition, int number, int position) {
	log_info(LOGGER, "--------------------------------");
	log_info(LOGGER, "Partition #%d", number);
	log_info(LOGGER, "Free: %s", partition->free ? "true" : "false");
	log_info(LOGGER, "Start: %d - %x", position, partition->start);
	log_info(LOGGER, "Size: %d", partition->size);
	log_info(LOGGER, "Buddy: %d - %x", xor_int_and_int(position, partition->size), xor_pointer_and_int(partition->start, partition->size));
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
	Partition* victim = list_get(occupied_partitions, 0);
	victim->free = true;
	return victim;
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

static bool _partition_at_position(uintptr_t position_to_compare, uintptr_t actual_position) {
	return position_to_compare == actual_position;
}

static void _free_partition(Partition* partition) {
	list_destroy(partition->notified_suscribers);
	free(partition->message);
	free(partition);
}
