#include "constants.h"

void init_config() {
	t_config* config = config_create("/home/utnso/tp-2020-1c-C90/broker/config/broker.config");

	IP = config_get_string_value(config, "IP_BROKER");
	PUERTO = config_get_string_value(config, "PUERTO_BROKER");
	TAMANO_MEMORIA = config_get_int_value(config, "TAMANO_MEMORIA");
	TAMANO_MINIMO_PARTICION = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	ALGORITMO_MEMORIA = config_get_string_value(config, "ALGORITMO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	ALGORITMO_PARTICION_LIBRE = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
	FRECUENCIA_COMPACTACION = config_get_int_value(config, "FRECUENCIA_COMPACTACION");
}

void init_queues() {
	NEW_QUEUE = queue_create();
	APPEARED_QUEUE = queue_create();
	GET_QUEUE = queue_create();
	LOCALIZED_QUEUE = queue_create();
	CATCH_QUEUE = queue_create();
	CAUGHT_QUEUE = queue_create();
}

void init_semaphores() {
	pthread_mutex_init(&MUTEX_NEW_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_APPEARED_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_GET_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_LOCALIZED_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_CATCH_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_CAUGHT_QUEUE, NULL);
	sem_init(&NEW_MESSAGES, 0, 0);
	sem_init(&APPEARED_MESSAGES, 0, 0);
	sem_init(&GET_MESSAGES, 0, 0);
	sem_init(&LOCALIZED_MESSAGES, 0, 0);
	sem_init(&CATCH_MESSAGES, 0, 0);
	sem_init(&CAUGHT_MESSAGES, 0, 0);
}
