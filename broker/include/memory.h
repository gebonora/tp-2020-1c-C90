#ifndef BROKER_INCLUDE_MEMORY_H_
#define BROKER_INCLUDE_MEMORY_H_

#include "constants.h"
#include <string.h>

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
void lru();
void fifo();
void best_fit();
void first_fit();

#endif /* BROKER_INCLUDE_MEMORY_H_ */
