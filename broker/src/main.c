#include "main.h"

static void _init_logger();
static void _init_config();
static void _init_semaphores();
static void _init_context();
static void _init_memory();
static void _init_dump();

int main(){
	_init_logger();
	_init_config();
	_init_semaphores();
	_init_context();
	_init_memory();
	_init_dump();
	init_server();
	return EXIT_SUCCESS;
}

/** PRIVATE FUNCTIONS **/

static void _init_logger() {
    LOGGER = log_create(LOGGER_PATH, "Broker Server", 1, LOG_LEVEL_INFO);
}

static void _init_config() {
	t_config* config = config_create("config/broker.config");

	IP = string_duplicate(config_get_string_value(config, "IP_BROKER"));
	PUERTO = string_duplicate(config_get_string_value(config, "PUERTO_BROKER"));
	ALGORITMO_MEMORIA = string_duplicate(config_get_string_value(config, "ALGORITMO_MEMORIA"));
	ALGORITMO_REEMPLAZO = string_duplicate(config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
	ALGORITMO_PARTICION_LIBRE = string_duplicate(config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE"));
	FRECUENCIA_COMPACTACION = config_get_int_value(config, "FRECUENCIA_COMPACTACION");

	if(string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		TAMANO_MINIMO_PARTICION = next_power_of_2(config_get_int_value(config, "TAMANO_MINIMO_PARTICION"));
		TAMANO_MEMORIA = next_power_of_2(config_get_int_value(config, "TAMANO_MEMORIA"));
	} else {
		TAMANO_MINIMO_PARTICION = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
		TAMANO_MEMORIA = config_get_int_value(config, "TAMANO_MEMORIA");
	}

	log_debug(LOGGER, "IP: %s", IP);
	log_debug(LOGGER, "PUERTO: %s", PUERTO);
	log_debug(LOGGER, "ALGORITMO_MEMORIA: %s", ALGORITMO_MEMORIA);
	log_debug(LOGGER, "ALGORITMO_REEMPLAZO: %s", ALGORITMO_REEMPLAZO);
	log_debug(LOGGER, "ALGORITMO_PARTICION_LIBRE: %s", ALGORITMO_PARTICION_LIBRE);
	log_debug(LOGGER, "FRECUENCIA_COMPACTACION: %d", FRECUENCIA_COMPACTACION);
	log_debug(LOGGER, "TAMANO_MEMORIA: %d", TAMANO_MEMORIA);
	log_debug(LOGGER, "TAMANO_MINIMO_PARTICION: %d", TAMANO_MINIMO_PARTICION);

	config_destroy(config);
}

static void _init_semaphores() {
	pthread_mutex_init(&MUTEX_MESSAGE_ID, NULL);
	pthread_mutex_init(&MUTEX_SUBSCRIBERS_BY_QUEUE, NULL);
	pthread_mutex_init(&MUTEX_READERS, NULL);
	pthread_mutex_init(&MUTEX_TIME, NULL);
	sem_init(&MEMORY, 0, 1);
}

static void _init_context() {
	MESSAGE_ID = 0;
	READERS = 0;
	TIME = 0;
	SUBSCRIBERS_BY_QUEUE = dictionary_create();
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "NEW", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "APPEARED", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "GET", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "LOCALIZED", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "CATCH", list_create());
	dictionary_put(SUBSCRIBERS_BY_QUEUE, "CAUGHT", list_create());
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
