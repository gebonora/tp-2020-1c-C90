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

t_log* logger;

char* IP;
char* PUERTO;
int TAMANO_MEMORIA;
int TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
int FRECUENCIA_COMPACTACION;


void iniciar_servidor(void);
void esperar_cliente(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);


#endif /* BROKER_INCLUDE_SERVER_H_ */
