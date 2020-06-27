/*
 * interface.h
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_INCLUDE_INTERFACE_H_
#define BROKER_INCLUDE_INTERFACE_H_

#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include "protocol.h"

Name* recv_name(int);
Coordinate* recv_coordinate(int);
void free_name(Name*);
t_list* recv_coordinates(int, bool);
Name* create_name(char*);
Get* create_get_pokemon(char*);
Coordinate* create_coordinate(uint32_t, uint32_t);
void free_coordinate(void*);

// CATCH, APPEARED
void* serialize_pokemon(Pokemon*, Operation, int);
int calculate_pokemon_bytes(Pokemon*);
int send_pokemon(Pokemon*, Operation, int);
Pokemon* recv_pokemon(int, bool);
void free_pokemon(Pokemon*);
Pokemon* create_pokemon(char*, uint32_t, uint32_t);

// NEW
void* serialize_new(New*, int);
int calculate_new_bytes(New*);
int send_new(New*, int);
New* recv_new(int);
void free_new(New*);
New* create_new_pokemon(char*, uint32_t, uint32_t, uint32_t);

// GET
void* serialize_get(Get*, int);
int calculate_get_bytes(Get*);
int send_get(Get*, int);
Get* recv_get(int);
void free_get(Get*);
Get* create_get_pokemon(char*);

// LOCALIZED
void* serialize_localized(Localized*, int);
int calculate_localized_bytes(Localized*);
int send_localized(Localized*, int);
Localized* recv_localized(int);
void free_localized(Localized*);

// CAUGHT
void* serialize_caught(Caught*, int);
int calculate_caught_bytes();
int send_caught(Caught*, int);
Caught* recv_caught(int);
Caught* create_caught_pokemon(uint32_t);

int create_connection(char*, char*);
char* get_operation_by_value(int);

#endif /* BROKER_INCLUDE_INTERFACE_H_ */
