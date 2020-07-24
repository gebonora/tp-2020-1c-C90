#ifndef BROKER_INCLUDE_BROKER_H_
#define BROKER_INCLUDE_BROKER_H_

#include "delibird/protocol.h"
#include "constants.h"
#include "delibird/interface.h"
#include "mem.h"

uint32_t get_id();
void send_message(void*, Operation, uint32_t, uint32_t);
void consumer_queue(Operation, Subscriber*);

#endif
