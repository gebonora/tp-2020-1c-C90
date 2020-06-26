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
	pthread_mutex_init(&MUTEX_MESSAGE_ID, NULL);
	pthread_mutex_init(&MUTEX_NEW_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_APPEARED_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_GET_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_LOCALIZED_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_CATCH_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_CAUGHT_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_SUBSCRIBERS_BY_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_MEMORY, NULL);

	sem_init(&NEW_MESSAGES, 0, 0);
	sem_init(&APPEARED_MESSAGES, 0, 0);
	sem_init(&GET_MESSAGES, 0, 0);
	sem_init(&LOCALIZED_MESSAGES, 0, 0);
	sem_init(&CATCH_MESSAGES, 0, 0);
	sem_init(&CAUGHT_MESSAGES, 0, 0);
	sem_init(&SUBSCRIBERS, 0, 0);
}

void init_context() {
	MESSAGE_ID = 0;
	SUBSCRIBERS_BY_QUEUE = dictionary_create();
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "NEW", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "APPEARED", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "GET", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "LOCALIZED", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "CATCH", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "CAUGHT", list_create());
}

void init_threads() {
	pthread_t thread_new;
	pthread_create(&thread_new, NULL,(void*)consumer_new_queue, NULL);
	pthread_detach(thread_new);

	pthread_t thread_get;
	pthread_create(&thread_get, NULL,(void*)consumer_get_queue, NULL);
	pthread_detach(thread_get);

	pthread_t thread_appeared;
	pthread_create(&thread_appeared, NULL,(void*)consumer_appeared_queue, NULL);
	pthread_detach(thread_appeared);

	pthread_t thread_localized;
	pthread_create(&thread_localized, NULL,(void*)consumer_localized_queue, NULL);
	pthread_detach(thread_localized);

	pthread_t thread_catch;
	pthread_create(&thread_catch, NULL,(void*)consumer_catch_queue, NULL);
	pthread_detach(thread_new);

	pthread_t thread_caught;
	pthread_create(&thread_caught, NULL,(void*)consumer_caught_queue, NULL);
	pthread_detach(thread_caught);
}

void init_memory() {
	memory->cache = malloc(TAMANO_MEMORIA);
	memory->partitions = list_create();

	// PRUEBA BEL FILTER
	/*
	create_partitions_test();
	log_info(LOGGER, "==============MEMORY============");
	show_memory_partitions();
	log_info(LOGGER, "==============FILTERED============");
	t_list* filtered = get_filtered_partitions();
	show_partitions(filtered);
	*/

	// PRUEBA SEBA LRU
	/*
	show_memory_partitions();
	log_info(LOGGER, "==============OCUPPIED============");
	t_list* occupied_partitions = get_occupied_partitions();
	show_partitions(occupied_partitions);
	log_info(LOGGER, "==============LRU============");
	Partition* partition = memory_lru();
	show_partition(partition);
	 */
}

void create_partitions_test() {
	list_add(memory->partitions, create_partition(0, 8, malloc(sizeof(uint32_t)), 0, create_message(NEW, 19, 16, 14)));
	sleep(1);
	list_add(memory->partitions, create_partition(17, 8, malloc(sizeof(uint32_t)), 8, create_message(CATCH, 20, -1, 8)));
	sleep(1);
	list_add(memory->partitions, create_partition(13, 8, malloc(sizeof(uint32_t)), 16, create_message(LOCALIZED, 13, 7, 25)));
	sleep(1);
	list_add(memory->partitions, create_partition(4, 4, malloc(sizeof(uint32_t)), 24, create_message(APPEARED, 12, -1, 45)));
	sleep(1);
	list_add(memory->partitions, create_partition(25, 4, malloc(sizeof(uint32_t)), 28, create_message(APPEARED, 12, -1, 45)));

}
