#ifndef BROKER_INCLUDE_MEM_H_
#define BROKER_INCLUDE_MEM_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include <delibird/utils/tiempo/Tiempo.h>
#include <delibird/interface.h>
#include "buddy.h"
#include "constants.h"
#include "dynamic.h"
#include "subscribers.h"

Partition* save_message(void*, Message*);
t_list* messages_from_operation(Operation, Subscriber*);
void lock_memory_for_read(char* context);
void unlock_memory_for_read(char* context);
void lock_memory_for_write(char* context);
void unlock_memory_for_write(char* context);

#endif /* BROKER_INCLUDE_MEM_H_ */
