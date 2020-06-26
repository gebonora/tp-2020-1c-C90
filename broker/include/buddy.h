#ifndef BROKER_INCLUDE_BUDDY_H_
#define BROKER_INCLUDE_BUDDY_H_

#include "constants.h"
#include <stdbool.h>
#include <commons/collections/list.h>

Partition* get_buddy(Partition* partition);
Partition* buddy_of(Partition* partition);
uint32_t xor(uint32_t position, uint32_t size);
uint32_t* starts_with(Partition* partition);
void save_to_cache_buddy_system(void*, Message*);

#endif /* BROKER_INCLUDE_BUDDY_H_ */
