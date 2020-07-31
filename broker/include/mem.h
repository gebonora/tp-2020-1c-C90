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
int lock_memory_for_read(char* context);
int unlock_memory_for_read(char* context);
int lock_memory_for_write(char* context);
int unlock_memory_for_write(char* context);

#endif /* BROKER_INCLUDE_MEM_H_ */
