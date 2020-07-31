#ifndef BROKER_INCLUDE_BROKER_H_
#define BROKER_INCLUDE_BROKER_H_

#include "delibird/interface.h"
#include "delibird/protocol.h"
#include "constants.h"
#include "mem.h"
#include "serialization.h"

uint32_t get_id();
void new_request(void*, Operation, uint32_t, uint32_t);
void new_suscription(Operation, Subscriber*);

#endif
