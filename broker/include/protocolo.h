/*
 * protocolo.h
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_INCLUDE_PROTOCOLO_H_
#define BROKER_INCLUDE_PROTOCOLO_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	NEW = 0,
	LOCALIZED = 1,
	GET = 2,
	APPEARED = 3,
	CATCH = 4,
	CAUGHT = 5
} Operation;

typedef struct {
	uint32_t sizeName;
	char* pokemonName;
	uint32_t posX;
	uint32_t posY;
} Catch;

typedef struct {
	uint32_t result;
} Caught;

typedef struct {
	uint32_t sizeName;
	char* pokemonName;
	uint32_t posX;
	uint32_t posY;
} Appeared;

typedef struct {
	Operation operationCode;
	void* buffer;
} Message;


#endif /* BROKER_INCLUDE_PROTOCOLO_H_ */
