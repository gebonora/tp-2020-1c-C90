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

int recvUint32(int);

Catch* recvCatch(int);
void sendCatch(Catch*, int);
int calcularBytesCatch(Catch*);
void* serializarCatch(Message*, int);


#endif /* BROKER_INCLUDE_INTERFACE_H_ */
