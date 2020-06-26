#include "arithmetic.h"

unsigned int next_power_of_2(unsigned int n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

uint32_t xor_int_and_int(uint32_t a, uint32_t b) {
	return (uint32_t) (unsigned) a ^ (unsigned) b;
}


uintptr_t xor_pointer_and_int(uintptr_t pointer, uint32_t value) {
	return pointer ^ value;
}
