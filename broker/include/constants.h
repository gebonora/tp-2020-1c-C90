#ifndef BROKER_INCLUDE_CONSTANTS_H_
#define BROKER_INCLUDE_CONSTANTS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <readline/readline.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <delibird/protocol.h>

#define DUMP_PATH "dump.log"
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
pthread_mutex_t MUTEX_MESSAGE_ID;
pthread_mutex_t MUTEX_READERS;
pthread_mutex_t MUTEX_TIME;
sem_t MEMORY;

t_dictionary* SUBSCRIBERS_BY_QUEUE;
pthread_mutex_t MUTEX_SUBSCRIBERS_BY_QUEUE;
sem_t SUBSCRIBERS;

int READERS;
uint32_t TIME;

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
	Process process;
	uint32_t id;
	int socket_subscriber;
} Subscriber;

typedef struct {
	uint32_t position; // posicion relativa dentro de la cache
	uintptr_t start; // puntero de la memoria cache
	uint32_t size; // tamanio de particion
	bool free; // si esta libre o no
	uint32_t creation_time; // timestamp del momento en que se creo
	uint32_t access_time; // timestamp del ultimo acceso a esta particion
	Message* message; // datos administrativos del mensaje (id, id correlacional, cod op)
	t_list* notified_suscribers; // suscriptores que ya devolvieron ACK para este mensaje
} Partition;

Memory* memory;

#endif
