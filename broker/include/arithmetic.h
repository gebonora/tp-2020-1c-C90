#ifndef BROKER_INCLUDE_ARITHMETIC_H_
#define BROKER_INCLUDE_ARITHMETIC_H_

#include <stdint.h>

#define MIN(x, y) ((x) < (y)) ? (x) : (y)
#define MAX(x, y) ((x) > (y)) ? (x) : (y)
#define MAX_PARTITION_SIZE(x) MAX(x, TAMANO_MINIMO_PARTICION)

uint32_t next_power_of_2(uint32_t);
uint32_t xor_int_and_int(uint32_t, uint32_t);
uintptr_t xor_pointer_and_int(uintptr_t, uint32_t);

#endif /* BROKER_INCLUDE_ARITHMETIC_H_ */
