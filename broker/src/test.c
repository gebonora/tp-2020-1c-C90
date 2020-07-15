#include "test.h"

static Message* _create_message(Operation, uint32_t, uint32_t, uint32_t);

void init_test() {
	/*
	Partition* first_one = list_get(memory->partitions, 0);
	uintptr_t start = first_one->start;

	list_add(memory->partitions, create_partition_with_message(0, start, 8, _create_message(NEW, 19, 16, 14)));
	sleep(1);
	start += 8;
	list_add(memory->partitions, create_partition_with_message(start, 8, _create_message(CATCH, 20, -1, 8)));
	sleep(1);
	start += 8;
	list_add(memory->partitions, create_partition_with_message(start, 8, _create_message(LOCALIZED, 13, 7, 25)));
	sleep(1);
	start += 8;
	list_add(memory->partitions, create_partition_with_message(start, 4, _create_message(APPEARED, 12, -1, 45)));
	sleep(1);
	start += 4;
	list_add(memory->partitions, create_partition_with_message(start, 4, _create_message(APPEARED, 12, -1, 45)));

	show_memory_partitions();

	// PRUEBA BEL FILTER
	/*
	create_partitions_test();
	log_info(LOGGER, "==============MEMORY============");
	show_memory_partitions();
	log_info(LOGGER, "==============FILTERED============");
	t_list* filtered = get_filtered_partitions();
	show_partitions(filtered);
	*/

	// PRUEBA SEBA LRU
	//create_partitions_test();
	//show_memory_partitions();
	//log_info(LOGGER, "==============OCUPPIED============");
	//t_list* occupied_partitions = get_occupied_partitions();
	//show_partitions(occupied_partitions);
	//log_info(LOGGER, "==============LRU============");
	//Partition* partition = choose_victim();
	//show_partition(partition);
}

/** PRIVATE FUNCTIONS **/

static Message* _create_message(Operation operation, uint32_t message_id, uint32_t correlational_id, uint32_t data_size) {
	Message* message = malloc(sizeof(Message));
	message->operation_code = operation;
	message->message_id = message_id;
	message->correlational_id = correlational_id;
	message->data_size = data_size;
	return message;
}
