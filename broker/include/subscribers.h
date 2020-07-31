#ifndef BROKER_INCLUDE_SUBSCRIBERS_H_
#define BROKER_INCLUDE_SUBSCRIBERS_H_

#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include "constants.h"

t_list* get_notified_subscribers_by_message_id(uint32_t message_id);
t_list* get_subscribers_by_operation(char* key);
void add_notified_subscriber(uint32_t message_id, Subscriber* subscriber);
bool was_notified(uint32_t message_id, void* subscriber);
bool was_not_notified(uint32_t message_id, void* subscriber);
Subscriber* create_or_update_subscriber(Operation operation, Process process, uint32_t id, int socket);
void lock_subscriber(Operation operation, Subscriber* subscriber);
void unlock_subscriber(Operation operation, Subscriber* subscriber);

#endif