#include "buddy.h"

Partition* get_buddy(Partition* partition) {
	return list_find(memory->partitions, starts_with(buddy_of(partition)));
}

Partition* buddy_of(Partition* partition) {
	return xor(partition->start, partition->size);
}

uint32_t xor(uint32_t position, uint32_t size) {
	return (uint32_t) (unsigned) position ^ (unsigned) size;
}

uint32_t* starts_with(Partition* partition) {
	return partition->start;
}
