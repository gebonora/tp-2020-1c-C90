#ifndef BROKER_INCLUDE_MEM_H_
#define BROKER_INCLUDE_MEM_H_

#include "constants.h"
#include "dynamic.h"
#include "buddy.h"
#include "mem_utils.h"
#include <string.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <delibird/utils/tiempo/Tiempo.h>
#include <delibird/interface.h>

#define BUDDY_SYSTEM "BS"
#define DYNAMIC_PARTITIONS "PARTICIONES"
#define FIRST_FIT "FF"
#define BEST_FIT "BF"
#define FIFO "FIFO"
#define LRU "LRU"

void save_message(void*, Operation, uint32_t, uint32_t);
Partition* test_lru(t_list*);
t_list* get_occupied_partitions();
t_list* get_sorted_partitions();

#endif /* BROKER_INCLUDE_MEM_H_ */
