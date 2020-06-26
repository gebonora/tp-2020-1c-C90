#include "mem_utils.h"

Message* create_message(Operation operation, uint32_t message_id, uint32_t correlational_id, uint32_t data_size) {
	Message* message = malloc(sizeof(Message));
	message->operation_code = operation;
	message->message_id = message_id;
	message->correlational_id = correlational_id;
	message->data_size = data_size;
	return message;
}

Partition* create_partition(uint32_t partition_number, uint32_t partition_size, uint32_t* partition_start, uint32_t position, Message* message) {
	Partition* partition = malloc(sizeof(Partition));
	partition->access_time = (int) ahoraEnTimeT() - partition_number; // borrar la resta
	partition->free = partition_number % 2 == 0 ? true : false;
	partition->number = partition_number;
	partition->size = partition_size;
	partition->start = partition_start;
	partition->position = position;
	partition->message = message;
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
