#ifndef BROKER_INCLUDE_MEM_UTILS_H_
#define BROKER_INCLUDE_MEM_UTILS_H_

#include <stdbool.h>
#include "constants.h"

Partition* create_partition(uint32_t, uint32_t, uint32_t*, uint32_t, Message*);
void show_partitions(t_list*);
void show_memory_partitions();
void show_partition(Partition*);
void show_message(Message*);

#endif /* BROKER_INCLUDE_MEM_UTILS_H_ */
