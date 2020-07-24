#include "mem_utils.h"

static Partition* _choose_victim(bool (*)(void*, void*));
static bool _is_occupied(Partition*);
static bool _less_access_time(Partition*, Partition*);
static bool _less_creation_time(Partition*, Partition*);
static bool _is_free(Partition*);
static t_list* _get_occupied_partitions();
static void _show_partition(Partition*, int);
static void _show_message(Message*);
static bool _greater_equals_and_free(uint32_t, Partition*);
static Partition* _find_partition(int, bool);
static t_list* greater_equals_and_free(uint32_t);
static bool _smaller_size(Partition*, Partition*);
static void _free_partition(Partition*);
static void _free_partition_attributes(Partition*);
static void _free_subscriber(Subscriber*);
static int _list_find_index(uintptr_t);
static bool _partition_with_start(uintptr_t, uintptr_t);

/** PUBLIC FUNCTIONS **/

uint32_t get_time() {
	pthread_mutex_lock(&MUTEX_TIME);
	uint32_t time = ++TIME;
	pthread_mutex_unlock(&MUTEX_TIME);
	return time;
}

Partition* find_partition(int size_to_fit) {
	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE, FIRST_FIT)) {
		return _find_partition(size_to_fit, false);
	} else {
		return _find_partition(size_to_fit, true);
	}
}

Partition* choose_victim() {
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, FIFO)) {
		log_debug(LOGGER, "Using FIFO");
		return _choose_victim(&_less_creation_time);
	} else {
		log_debug(LOGGER, "Using LRU");
		return _choose_victim(&_less_access_time);
	}
}

Partition* create_partition(uint32_t position, uintptr_t start, uint32_t size) {
	uint64_t now = get_time();
	Partition* partition = malloc(sizeof(Partition));
	partition->creation_time = now;
	partition->access_time = now;
	partition->free = true;
	partition->size = size;
	partition->position = position;
	partition->start = start;
	partition->notified_suscribers = list_create();
	return partition;
}

Partition* create_partition_with_message(uint32_t position, uintptr_t start, uint32_t size, Message* message) {
	Partition* partition = create_partition(position, start, size);
	partition->message = message;
	return partition;
}

void replace_partition_at(uintptr_t start_to_replace, Partition* replacement) {
	list_replace_and_destroy_element(memory->partitions, _list_find_index(start_to_replace), replacement, &_free_partition);
}

void remove_partition_at(uintptr_t start_to_delete) {
	list_remove_and_destroy_element(memory->partitions, _list_find_index(start_to_delete), &_free_partition);
}

void add_partition_next_to(uintptr_t start, Partition* partition_to_add) {
	list_add_in_index(memory->partitions, NEXT(_list_find_index(start)), partition_to_add);
}

Partition* find_partition_at(uintptr_t start_to_find) {
	bool _inline_partition_with_start(Partition* partition) {
		return _partition_with_start(start_to_find, partition->start);
	}

	return list_find(memory->partitions, &_inline_partition_with_start);
}

t_list* get_occupied_partitions() {
	return list_filter(memory->partitions, &_is_occupied);
}

t_list* get_free_partitions() {
	return list_filter(memory->partitions, &_is_free);
}


void show_partitions_with_index(t_list* partitions) {
	log_info(LOGGER, "--------Partitions Size: %d-------", partitions->elements_count);

	Partition* partition;

	// nro particion, direccion memoria comienzo, direccion fin, ocupada(X) libre(L), size,LRU, cola, id
	for(int index = 0, number = 1; index < partitions->elements_count; index++, number++) {
		partition = list_get(partitions, index);
		_show_partition(partition, number);
	}
}

/** PRIVATE FUNCTIONS **/

static Partition* _find_partition(int desired_size, bool best) {
	t_list* potential_partitions = greater_equals_and_free(desired_size);
	if(best) list_sort(potential_partitions, &_smaller_size);
	Partition* partition = list_get(potential_partitions, 0);
	list_destroy(potential_partitions);
	return partition;
}

static int _list_find_index(uintptr_t start_to_find) {
	log_debug(LOGGER, "Finding index for partition at: %x", start_to_find);

	t_link_element *element = memory->partitions->head;
	int index = 0;

	bool _inline_partition_with_start(Partition* partition) {
		return _partition_with_start(start_to_find, partition->start);
	}

	while (element != NULL && !_inline_partition_with_start(element->data)) {
		element = element->next;
		index++;
	}

	log_debug(LOGGER, "Partition index: %d", index);
	return index;

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

static void _show_partition(Partition* partition, int number) {
	log_info(LOGGER, "--------------------------------");
	log_info(LOGGER, "Partition #%d", number);
	log_info(LOGGER, "Free: %s", partition->free ? "true" : "false");
	log_info(LOGGER, "Size: %d", partition->size);
	log_info(LOGGER, "Position: %d - %d", partition->position, partition->position + partition->size -1);
	log_info(LOGGER, "Start: %x (%d) - %x (%d)", partition->start, partition->start, partition->start + partition->size - 1, partition->start + partition->size - 1);
	log_info(LOGGER, "Buddy: %d", xor_int_and_int(partition->position, partition->size));
	log_info(LOGGER, "Creation time: %d", partition->creation_time);
	log_info(LOGGER, "Last access: %d", partition->access_time);
	if(!partition->free) {
		_show_message(partition->message);
	}
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

static Partition* _choose_victim(bool (*comparator)(void*, void*)) {
	log_debug(LOGGER, "Getting occupied partitions");
	t_list* occupied_partitions = _get_occupied_partitions();
	log_debug(LOGGER, "Sorting occupied partitions");
	list_sort(occupied_partitions, comparator);
	log_debug(LOGGER, "Getting first partition");
	Partition* victim = list_get(occupied_partitions, 0);
	list_destroy(occupied_partitions);
	log_debug(LOGGER, "Victim Start: %x (%d), Position %d, Size: %d, Creation: %d, Access: %d", victim->start, victim->start, victim->position, victim->size, victim->creation_time, victim->access_time);
	log_debug(LOGGER, "Setting victim free to true");;
	victim->free = true;
	log_debug(LOGGER, "Free partition attributes");
	_free_partition_attributes(victim);
	list_destroy(occupied_partitions);
	return victim;
}

static bool _is_occupied(Partition* partition) {
	return !partition->free;
}

static bool _is_free(Partition* partition) {
	return partition->free;
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

static bool _partition_with_start(uintptr_t start_to_compare, uintptr_t actual_start) {
	return start_to_compare == actual_start;
}

static void _free_partition(Partition* partition) {
	log_debug(LOGGER, "Free subscribers");
	list_destroy_and_destroy_elements(partition->notified_suscribers, &_free_subscriber);
	free(partition);
}

static void _free_partition_attributes(Partition* partition) {
	log_debug(LOGGER, "Cleaning subscribers");
	list_clean(partition->notified_suscribers);
	log_debug(LOGGER, "Free message");
	free(partition->message);
}

static void _free_subscriber(Subscriber* subscriber) {
	free(subscriber);
}
