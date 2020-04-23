#ifndef BROKER_INCLUDE_PROTOCOLO_H_
#define BROKER_INCLUDE_PROTOCOLO_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>

typedef enum {
	NEW = 0, LOCALIZED = 1, GET = 2, APPEARED = 3, CATCH = 4, CAUGHT = 5
} Operation;

typedef struct {
	uint32_t pos_x;
	uint32_t pos_y;
} Coordinate;

typedef struct {
	uint32_t size;
	char* value;
} Name;

typedef struct {
	Name* name;
	t_list* coordinates;
} Pokemon; // incluye Catch, Appeared

typedef struct {
	Pokemon* pokemon;
	int quantity;
} New;

typedef struct {
	Name* name;
} Get;

typedef struct {
	Pokemon* pokemon;
	int coordinates_quantity;
} Localized;

typedef struct {
	uint32_t result;
} Caught;

typedef struct {
	Operation operation_code;
	void* buffer;
} Message;

#endif /* BROKER_INCLUDE_PROTOCOLO_H_ */
