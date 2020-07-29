#ifndef BROKER_INCLUDE_BROKER_H_
#define BROKER_INCLUDE_BROKER_H_

#include "delibird/protocol.h"
#include "constants.h"
#include "delibird/interface.h"
#include "mem.h"

uint32_t get_id();
void send_message_from_new_request(void*, Operation, uint32_t, uint32_t);
void send_message_from_suscription(Operation, Subscriber*);
uint32_t get_subscriber_identifier(Subscriber*);

#endif
