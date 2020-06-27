#include "test.h"

//Partition* test_lru(t_list*);

void create_partitions_test() {

	/*
	uintptr_t start = malloc(sizeof(uintptr_t));

	list_add(memory->partitions, create_partition(0, 8, start, 0, create_message(NEW, 19, 16, 14)));
	sleep(1);
	start += 8;
	list_add(memory->partitions, create_partition(17, 8, start, 8, create_message(CATCH, 20, -1, 8)));
	sleep(1);
	start += 8;
	list_add(memory->partitions, create_partition(13, 8, start, 16, create_message(LOCALIZED, 13, 7, 25)));
	sleep(1);
	start += 8;
	list_add(memory->partitions, create_partition(4, 4, start, 24, create_message(APPEARED, 12, -1, 45)));
	sleep(1);
	start += 4;
	list_add(memory->partitions, create_partition(25, 4, start, 28, create_message(APPEARED, 12, -1, 45)));
	*/

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
