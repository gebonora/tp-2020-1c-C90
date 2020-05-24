#ifndef BROKER_INCLUDE_CONSTANTS_H_
#define BROKER_INCLUDE_CONSTANTS_H_

#include <commons/collections/queue.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>

char* IP;
char* PUERTO;
int TAMANO_MEMORIA;
int TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
int FRECUENCIA_COMPACTACION;
char* ALGORITMO_PARTICION_LIBRE;

t_log* LOGGER;
t_queue* NEW_QUEUE;
t_queue* APPEARED_QUEUE;
t_queue* GET_QUEUE;
t_queue* LOCALIZED_QUEUE;
t_queue* CATCH_QUEUE;
t_queue* CAUGHT_QUEUE;
pthread_mutex_t MUTEX_NEW_QUEUE;
pthread_mutex_t MUTEX_APPEARED_QUEUE;
pthread_mutex_t MUTEX_GET_QUEUE;
pthread_mutex_t MUTEX_LOCALIZED_QUEUE;
pthread_mutex_t MUTEX_CATCH_QUEUE;
pthread_mutex_t MUTEX_CAUGHT_QUEUE;
sem_t NEW_MESSAGES;
sem_t APPEARED_MESSAGES;
sem_t GET_MESSAGES;
sem_t LOCALIZED_MESSAGES;
sem_t CATCH_MESSAGES;
sem_t CAUGHT_MESSAGES;

void init_config();
void init_queues();
void init_semaphores();

#endif
