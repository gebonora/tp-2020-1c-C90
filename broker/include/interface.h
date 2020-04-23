/*
 * interface.h
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_INCLUDE_INTERFACE_H_
#define BROKER_INCLUDE_INTERFACE_H_

#include "protocolo.h"
#include <string.h>
#include <sys/socket.h>

uint32_t recv_uint32(int);
Name* recv_name(int);
Coordinate* recv_coordinate(int);
void free_name(Name*);

// CATCH, APPEARED
void* serialize_pokemon(Pokemon*, Operation, int);
int calculate_pokemon_bytes(Pokemon*);
void send_pokemon(Pokemon*, Operation, int);
Pokemon* recv_pokemon(int);
void free_pokemon(Pokemon*);

// NEW
void* serialize_new(New*, int);
int calculate_new_bytes(New*);
void send_new(New*, int);
New* recv_new(int);
void free_new(New*);

// GET
void* serialize_get(Get*, int);
int calculate_get_bytes(Get*);
void send_get(Get*, int);
Get* recv_get(int);
void free_get(Get*);

#endif /* BROKER_INCLUDE_INTERFACE_H_ */
