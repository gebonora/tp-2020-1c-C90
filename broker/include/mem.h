#ifndef BROKER_INCLUDE_MEM_H_
#define BROKER_INCLUDE_MEM_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include <delibird/utils/tiempo/Tiempo.h>
#include <delibird/interface.h>
#include "constants.h"
#include "dynamic.h"
#include "buddy.h"

void save_message(void*, Operation, uint32_t, uint32_t);
t_list* messages_from_operation(Operation);

#endif /* BROKER_INCLUDE_MEM_H_ */
