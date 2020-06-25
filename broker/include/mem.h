#ifndef BROKER_INCLUDE_MEM_H_
#define BROKER_INCLUDE_MEM_H_

#include "constants.h"
#include "dynamic.h"
#include "buddy.h"
#include <string.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <delibird/utils/tiempo/Tiempo.h>
/*
void add();
void choose_victim();
void delete_data();
void compact();
void delete_partition();
void dump();

int find_free_partition();
void save_data();

//func priv
void save_buddy_system();
void save_dynamic_partitions();
void fifo();
void best_fit();
void first_fit();
*/
Partition* create_partition(uint32_t partition_number, uint32_t partition_size, uint32_t* partition_start, uint32_t position, Message* message);
Message* create_message(Operation operation, uint32_t message_id, uint32_t correlational_id, uint32_t data_size);
void show_partitions();
void show_partition(Partition* partition);
void show_message(Message* message);
t_list* get_filtered_partitions(uint32_t);
bool greater_equals_and_free(uint32_t, Partition*);
Partition* lru(t_list* partitions);
Partition* memory_lru();
bool less_access_time(Partition* partition_a, Partition* partition_b);
bool is_occupied(Partition* partition);
t_list* get_occupied_partitions();
t_list* get_sorted_partitions();

#endif /* BROKER_INCLUDE_MEM_H_ */
