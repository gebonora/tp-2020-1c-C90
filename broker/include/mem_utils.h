#ifndef BROKER_INCLUDE_MEM_UTILS_H_
#define BROKER_INCLUDE_MEM_UTILS_H_

#include <stdbool.h>
#include "arithmetic.h"
#include "constants.h"

#define MAX_PARTITION_SIZE(x) MAX(x, TAMANO_MINIMO_PARTICION)

Partition* find_partition(int);
Partition* choose_victim();
Partition* create_partition(uint32_t, uintptr_t, uint32_t);
Partition* create_partition_with_message(uint32_t, uintptr_t, uint32_t, Message*);
void replace_partition_at(uintptr_t, Partition*);
void remove_partition_at(uintptr_t);
void add_partition_next_to(uintptr_t, Partition*);
Partition* find_partition_at(uintptr_t);
void show_partitions_with_index(t_list*);

#endif /* BROKER_INCLUDE_MEM_UTILS_H_ */
