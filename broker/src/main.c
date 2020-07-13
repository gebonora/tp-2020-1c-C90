#include "main.h"

static void _init_logger();
static void _init_config();
static void _init_queues();
static void _init_semaphores();
static void _init_context();
static void _init_threads();
static void _init_memory();
static void _init_dump();

int main(){
	_init_logger();
	_init_config();
	_init_queues();
	_init_semaphores();
	_init_context();
	_init_threads();
	_init_memory();
	_init_dump();
	// TODO: esto borrarlo luego de los tests
	//init_test();
	init_server();
	return EXIT_SUCCESS;
}

/** PRIVATE FUNCTIONS **/

static void _init_logger() {
	// TODO: volver a level info
    LOGGER = log_create(LOGGER_PATH, "Broker Server", 1, LOG_LEVEL_DEBUG);
}

static void _init_config() {
	t_config* config = config_create("/home/utnso/tp-2020-1c-C90/broker/config/broker.config");

	IP = config_get_string_value(config, "IP_BROKER");
	PUERTO = config_get_string_value(config, "PUERTO_BROKER");
	ALGORITMO_MEMORIA = config_get_string_value(config, "ALGORITMO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	ALGORITMO_PARTICION_LIBRE = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
	FRECUENCIA_COMPACTACION = config_get_int_value(config, "FRECUENCIA_COMPACTACION");

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		TAMANO_MINIMO_PARTICION = next_power_of_2(config_get_int_value(config, "TAMANO_MINIMO_PARTICION"));
		TAMANO_MEMORIA = next_power_of_2(config_get_int_value(config, "TAMANO_MEMORIA"));
	} else {
		TAMANO_MINIMO_PARTICION = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
		TAMANO_MEMORIA = config_get_int_value(config, "TAMANO_MEMORIA");
	}
}

static void _init_queues() {
	NEW_QUEUE = queue_create();
	APPEARED_QUEUE = queue_create();
	GET_QUEUE = queue_create();
	LOCALIZED_QUEUE = queue_create();
	CATCH_QUEUE = queue_create();
	CAUGHT_QUEUE = queue_create();
}

static void _init_semaphores() {
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

static void _init_context() {
	MESSAGE_ID = 0;
	SUBSCRIBERS_BY_QUEUE = dictionary_create();
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "NEW", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "APPEARED", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "GET", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "LOCALIZED", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "CATCH", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "CAUGHT", list_create());
}

static void _init_threads() {
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

static void _init_memory() {
	memory = malloc(sizeof(Memory));
	memory->cache = malloc(TAMANO_MEMORIA);
	memory->partitions = list_create();
	list_add(memory->partitions, create_partition(0, memory->cache, TAMANO_MEMORIA));
	log_debug(LOGGER, "Memory Cache: %x (%d), Size: %d", memory->cache, memory->cache, TAMANO_MEMORIA);
}

static void _init_dump() {
	if(signal(SIGUSR1, dump_handler) == SIG_ERR) {
		log_info(LOGGER, "Error catching signal SIGUSR1");
	}
	if(signal(SIGUSR2, dump_handler) == SIG_ERR) {
		log_info(LOGGER, "Error catching signal SIGUSR2");
	}
}
