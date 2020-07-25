#ifndef BROKER_INCLUDE_BUDDY_H_
#define BROKER_INCLUDE_BUDDY_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include "arithmetic.h"
#include "constants.h"
#include "mem_utils.h"

Partition* save_to_cache_buddy_system(void*, Message*);

#endif /* BROKER_INCLUDE_BUDDY_H_ */
