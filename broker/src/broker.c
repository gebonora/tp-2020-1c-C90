#include "broker.h"

uint32_t get_id() {
	pthread_mutex_lock(&MUTEX_MESSAGE_ID);
	uint32_t generated_id = ++MESSAGE_ID;
	pthread_mutex_unlock(&MUTEX_MESSAGE_ID);
	return generated_id;
}

void consumer_new_queue() {
	sem_wait(&NEW_MESSAGES);
	pthread_mutex_lock(&MUTEX_NEW_QUEUE);
	New* new = queue_pop(NEW_QUEUE);
	pthread_mutex_unlock(&MUTEX_NEW_QUEUE);
	log_info(LOGGER, "Saque de la cola: %s", new->pokemon->name->value);

}

void consumer_appeared_queue() {
	sem_wait(&APPEARED_MESSAGES);
	pthread_mutex_lock(&MUTEX_APPEARED_QUEUE);
	Pokemon* appeared = queue_pop(APPEARED_QUEUE);
	pthread_mutex_unlock(&MUTEX_APPEARED_QUEUE);
	log_info(LOGGER, "Saque de la cola: %s", appeared->name->value);

}

void consumer_localized_queue() {
	sem_wait(&LOCALIZED_MESSAGES);
	pthread_mutex_lock(&MUTEX_LOCALIZED_QUEUE);
	Localized* localized = queue_pop(LOCALIZED_QUEUE);
	pthread_mutex_unlock(&MUTEX_LOCALIZED_QUEUE);
	log_info(LOGGER, "Saque de la cola: %s", localized->pokemon->name->value);

}

void consumer_get_queue() {
	sem_wait(&GET_MESSAGES);
	pthread_mutex_lock(&MUTEX_GET_QUEUE);
	Get* get = queue_pop(GET_QUEUE);
	pthread_mutex_unlock(&MUTEX_GET_QUEUE);
	log_info(LOGGER, "Saque de la cola: %s", get->name->value);

}

void consumer_catch_queue() {
	sem_wait(&CATCH_MESSAGES);
	pthread_mutex_lock(&MUTEX_CATCH_QUEUE);
	Pokemon* catch = queue_pop(CATCH_QUEUE);
	pthread_mutex_unlock(&MUTEX_CATCH_QUEUE);
	log_info(LOGGER, "Saque de la cola: %s", catch->name->value);

}

void consumer_caught_queue() {
	sem_wait(&CAUGHT_MESSAGES);
	pthread_mutex_lock(&MUTEX_CAUGHT_QUEUE);
	Caught* caught = queue_pop(CAUGHT_QUEUE);
	pthread_mutex_unlock(&MUTEX_CAUGHT_QUEUE);
	log_info(LOGGER, "Saque de la cola: %d", caught->result);

}

void init_dump() {
	if(signal(SIGUSR1, dump_handler) == SIG_ERR) {
		log_info(LOGGER, "Error catching signal SIGUSR1");
	}
}

void dump_handler(int signum) {
	if(signum == SIGUSR1) {
		log_info(LOGGER, "Received SIGUSR1");
		create_dump();
		log_info(LOGGER, "dump.log file created");
	}
}

void create_dump() {
    FILE * fp;
	fp = fopen("dump.log", "w+");
	fprintf(fp, "Dump: %s\n", current_date_time_as_string());
	fprintf(fp, "Particion 1: ...");
	fclose(fp);
}
