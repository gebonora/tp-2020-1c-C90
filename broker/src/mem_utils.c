#include "mem_utils.h"

static Partition* _choose_victim(bool(*condition)(void*));
static bool _is_occupied(Partition*);
static bool _less_access_time(Partition*, Partition*);
static bool _less_creation_time(Partition*, Partition*);
static t_list* _get_occupied_partitions();


// TODO: ver si lo movemos
t_list* get_occupied_partitions();
t_list* get_sorted_partitions();

// TODO: tiene que exponerse, chau static


// siempre que mato una particion, tengo que consolidar, siempre
Partition* choose_victim() {
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO, FIFO)) {
		return _choose_victim(&_less_creation_time);
	} else {
		return _choose_victim(&_less_access_time);
	}
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
	list_iterate(partitions, &show_partition);
}

void show_memory_partitions() {
	log_info(LOGGER, "--------------------------------");
	log_info(LOGGER, "Partitions size: %d", memory->partitions->elements_count);
	list_iterate(memory->partitions, &show_partition);
}

void show_partition(Partition* partition) {
	log_info(LOGGER, "Partition #%d", partition->number);
	log_info(LOGGER, "Free: %s", partition->free ? "true" : "false");
	log_info(LOGGER, "Start: %d - %x", partition->position, partition->start);
	log_info(LOGGER, "Size: %d", partition->size);
	log_info(LOGGER, "Buddy: %d - %x", xor_int_and_int(partition->position, partition->size), xor_pointer_and_int(partition->start, partition->size));
	log_info(LOGGER, "Last access: %d", partition->access_time);
	show_message(partition->message);
	log_info(LOGGER, "--------------------------------");
}

void show_message(Message* message) {
	log_info(LOGGER, "Message Queue: %s", get_operation_by_value(message->operation_code));
	log_info(LOGGER, "Message ID: %d", message->message_id);
	log_info(LOGGER, "Correlative ID: %d", message->correlational_id);
	log_info(LOGGER, "Message Size: %d", message->data_size);
}
