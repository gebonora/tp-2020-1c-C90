#ifndef BROKER_INCLUDE_DYNAMIC_H_
#define BROKER_INCLUDE_DYNAMIC_H_

#include "constants.h"

void compact();
void save_to_cache_dynamic_partitions(void*, Message*);
t_list* get_filtered_partitions(uint32_t);

#endif /* BROKER_INCLUDE_DYNAMIC_H_ */
