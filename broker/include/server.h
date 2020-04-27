#ifndef BROKER_INCLUDE_SERVER_H_
#define BROKER_INCLUDE_SERVER_H_


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>

#include "include/interface.h"
#include "include/protocol.h"

t_log* logger;

char* IP;
char* PUERTO;
int TAMANO_MEMORIA;
int TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
int FRECUENCIA_COMPACTACION;
char* ALGORITMO_PARTICION_LIBRE;


void iniciar_servidor(void);
void esperar_cliente(int);
void process_request(int, int);
void serve_client(int);


#endif /* BROKER_INCLUDE_SERVER_H_ */
