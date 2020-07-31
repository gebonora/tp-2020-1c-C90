#include "mem_utils.h"

static Partition* _choose_victim(bool (*)(void*, void*));
static bool _is_occupied(void*);
static bool _less_access_time(void*, void*);
static bool _less_creation_time(void*, void*);
static bool _is_free(void*);
static t_list* _get_occupied_partitions();
static bool _greater_equals_and_free(uint32_t, Partition*);
static Partition* _find_partition(int, bool);
static t_list* greater_equals_and_free(uint32_t);
static bool _smaller_size(void*, void*);
static void _free_partition(void*);
static void _free_partition_attributes(Partition*);
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
	uint32_t now = -1;
	Partition* partition = malloc(sizeof(Partition));
	partition->creation_time = now;
	partition->access_time = now;
	partition->free = true;
	partition->size = size;
	partition->position = position;
	partition->start = start;
	pthread_mutex_init(&partition->mutex, NULL);
	return partition;
}

Partition* create_partition_with_message(uint32_t position, uintptr_t start, uint32_t size, Message* message) {
	Partition* partition = create_partition(position, start, size);
	partition->message = message;
	return partition;
}

void replace_partition_at(uintptr_t start_to_replace, Partition* replacement) {
	list_replace_and_destroy_element(memory->partitions, _list_find_index(start_to_replace), replacement, _free_partition);
}

void remove_partition_at(uintptr_t start_to_delete) {
	list_remove_and_destroy_element(memory->partitions, _list_find_index(start_to_delete), _free_partition);
}

void add_partition_next_to(uintptr_t start, Partition* partition_to_add) {
	list_add_in_index(memory->partitions, NEXT(_list_find_index(start)), partition_to_add);
}

Partition* find_partition_at(uintptr_t start_to_find) {
	bool _inline_partition_with_start(void* e) {
		Partition* partition = e;
		return _partition_with_start(start_to_find, partition->start);
	}

	return list_find(memory->partitions, _inline_partition_with_start);
}

t_list* get_occupied_partitions() {
	return list_filter(memory->partitions, _is_occupied);
}

t_list* get_free_partitions() {
	return list_filter(memory->partitions, _is_free);
}

/** PRIVATE FUNCTIONS **/

static Partition* _find_partition(int desired_size, bool best) {
	t_list* potential_partitions = greater_equals_and_free(desired_size);
	if(best) list_sort(potential_partitions, _smaller_size);
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
	bool _inline_greater_equals_and_free(void* e) {
		Partition* partition = e;
		return _greater_equals_and_free(size_to_compare, partition);
	}
	return list_filter(memory->partitions, _inline_greater_equals_and_free);
}

static bool _smaller_size(void* e1, void* e2) {
	Partition* partition_1 = e1;
	Partition* partition_2 = e2;
	return partition_1->size < partition_2->size;
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
	log_info(LOGGER, "Victim Position %d (%x), Size: %d, Creation: %d, Access: %d, Message Id: %d", victim->position, victim->start, victim->size, victim->creation_time, victim->access_time, victim->message->message_id);
	log_debug(LOGGER, "Setting victim free to true");;
	victim->free = true;
	log_debug(LOGGER, "Free partition attributes");
	_free_partition_attributes(victim);
	return victim;
}

static bool _is_occupied(void* e) {
	Partition* partition = e;
	return !partition->free;
}

static bool _is_free(void* e) {
	Partition* partition = e;
	return partition->free;
}

static bool _less_access_time(void* e1, void* e2) {
	Partition* partition_a = e1;
		Partition* partition_b = e2;
	return partition_a->access_time < partition_b->access_time;
}

static bool _less_creation_time(void* e1, void* e2) {
	Partition* partition_a = e1;
	Partition* partition_b = e2;
	return partition_a->creation_time < partition_b->creation_time;
}

static t_list* _get_occupied_partitions() {
	return list_filter(memory->partitions, &_is_occupied);
}

static bool _partition_with_start(uintptr_t start_to_compare, uintptr_t actual_start) {
	return start_to_compare == actual_start;
}

static void _free_partition(void* e) {
	Partition* partition = e;
	free(partition);
}

static void _free_partition_attributes(Partition* partition) {
	free(partition->message);
}
