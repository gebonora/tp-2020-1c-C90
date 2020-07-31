#ifndef BROKER_INCLUDE_SERIALIZATION_H_
#define BROKER_INCLUDE_SERIALIZATION_H_

#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <delibird/interface.h>
#include "constants.h"

int calculate_message_size(void* data, Operation operation);
void* serialize_data(void* data, Operation operation, int bytes);
void* copy_message_from_memory(Partition* partition, Operation operation, int bytes);

#endif