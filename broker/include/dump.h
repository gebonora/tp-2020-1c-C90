#ifndef BROKER_INCLUDE_DUMP_H_
#define BROKER_INCLUDE_DUMP_H_

#include <signal.h>
#include "delibird/utils/tiempo/Tiempo.h"
#include "delibird/interface.h"
#include "arithmetic.h"
#include "constants.h"

#define DUMP_PATH "dump.log"

void dump_handler(int);

#endif
