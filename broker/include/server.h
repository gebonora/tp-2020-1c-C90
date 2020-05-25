#ifndef BROKER_INCLUDE_SERVER_H_
#define BROKER_INCLUDE_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "broker.h"
#include "constants.h"
#include "include/interface.h"
#include "include/protocol.h"

void productor();
void consumidor();
void laburar();
void init_server();
void esperar_cliente(int);
void process_request(int, int);
void serve_client(int);

#endif /* BROKER_INCLUDE_SERVER_H_ */
