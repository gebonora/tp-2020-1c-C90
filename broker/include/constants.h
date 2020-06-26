#ifndef BROKER_INCLUDE_CONSTANTS_H_
#define BROKER_INCLUDE_CONSTANTS_H_

#include <commons/collections/queue.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <delibird/protocol.h>
#include "arithmetic.h"
#include "broker.h"

#define LOGGER_PATH "/home/utnso/tp-2020-1c-C90/broker/logs/server.log"
#define BUDDY_SYSTEM "BS"
#define DYNAMIC_PARTITIONS "PARTICIONES"
#define FIRST_FIT "FF"
#define BEST_FIT "BF"
#define FIFO "FIFO"
#define LRU "LRU"

char* IP;
char* PUERTO;
int TAMANO_MEMORIA;
int TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
int FRECUENCIA_COMPACTACION;
char* ALGORITMO_PARTICION_LIBRE;

uint32_t MESSAGE_ID;
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
pthread_mutex_t MUTEX_MESSAGE_ID;
pthread_mutex_t MUTEX_MEMORY;
sem_t NEW_MESSAGES;
sem_t APPEARED_MESSAGES;
sem_t GET_MESSAGES;
sem_t LOCALIZED_MESSAGES;
sem_t CATCH_MESSAGES;
sem_t CAUGHT_MESSAGES;

t_dictionary* SUBSCRIBERS_BY_QUEUE;
pthread_mutex_t MUTEX_SUBSCRIBERS_BY_QUEUE;
sem_t SUBSCRIBERS;


typedef struct {
	void* cache;
	t_list* partitions;
} Memory;

typedef struct {
	Operation operation_code;
	uint32_t message_id;
	uint32_t correlational_id;
	uint32_t data_size;
} Message;

typedef struct {
	Process* process;
	uint32_t id;
	int socket;
} Suscriber;

typedef struct {
	uint32_t position; // posicion relativa dentro de la cache
	uintptr_t start; // puntero de la memoria cache
	uint32_t number; // numero de particion
	uint32_t size; // tamanio de particion
	bool free; // si esta libre o no
	uint32_t buddy; // numero de particion de su buddy
	uint32_t creation_time;
	uint32_t access_time; // timestamp del ultimo acceso a esta particion
	Message* message; // datos administrativos del mensaje (id, id correlacional, cod op)
	t_list* notified_suscribers; // suscriptores que ya devolvieron ACK para este mensaje
} Partition;

Memory* memory;

void init_logger();
void init_config();
void init_queues();
void init_semaphores();
void init_context();
void init_threads();
void init_memory();
void create_partitions_test();

#endif
