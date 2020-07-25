
#ifndef BROKER_INCLUDE_ARITHMETIC_H_
#define BROKER_INCLUDE_ARITHMETIC_H_

#include <stdint.h>
#include <stdio.h>

#define MIN(x, y) ((x) < (y)) ? (x) : (y)
#define MAX(x, y) ((x) > (y)) ? (x) : (y)
#define SUM(x, y) (x + y)
#define NEXT(n) (n + 1)
#define HALF(n) (n / 2)

uint32_t next_power_of_2(uint32_t);
uint32_t xor_int_and_int(uint32_t, uint32_t);

#endif /* BROKER_INCLUDE_ARITHMETIC_H_ */
