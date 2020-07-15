#ifndef BROKER_INCLUDE_BROKER_H_
#define BROKER_INCLUDE_BROKER_H_

#include "delibird/protocol.h"
#include "constants.h"
#include "delibird/interface.h"
#include "mem.h"

uint32_t get_id();
void consumer_new_queue();
void consumer_appeared_queue();
void consumer_get_queue();
void consumer_localized_queue();
void consumer_catch_queue();
void consumer_caught_queue();

#endif
