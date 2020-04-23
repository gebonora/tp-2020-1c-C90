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
void* serialize_pokemon(Pokemon*, Operation, int);
int calculate_bytes(Pokemon*);
void send_pokemon(Pokemon*, Operation, int);
Pokemon* recv_pokemon(int);

#endif /* BROKER_INCLUDE_INTERFACE_H_ */
