#ifndef BROKER_INCLUDE_PROTOCOL_H_
#define BROKER_INCLUDE_PROTOCOL_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>

typedef enum {
	GAMEBOY = 1, TEAM = 2, GAMECARD = 3, BROKER = 4, SUSCRIPTOR = 5
} Process;

typedef enum {
	NEW = 1, LOCALIZED = 2, GET = 3, APPEARED = 4, CATCH = 5, CAUGHT = 6, SUBSCRIBE = 7
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
	uint32_t quantity;
} New;

typedef struct {
	Name* name;
} Get;

typedef struct {
	Pokemon* pokemon;
	uint32_t coordinates_quantity;
} Localized;

typedef struct {
	uint32_t result;
} Caught;

typedef struct {
	Operation operation_code;
	void* buffer;
	uint32_t id_correlational;
} Message;

typedef struct {
	uint32_t id;
	Message* message;
	t_list* sent;
	t_list* confirmed;
} Memory_Message;

#endif /* BROKER_INCLUDE_PROTOCOL_H_ */
