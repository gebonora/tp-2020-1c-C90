#include "broker.h"

uint32_t get_id() {
	pthread_mutex_lock(&MUTEX_MESSAGE_ID);
	uint32_t generated_id = ++MESSAGE_ID;
	pthread_mutex_unlock(&MUTEX_MESSAGE_ID);
	return generated_id;
}

// TODO: readaptar para que en lugar de usar las queues, recorra la memory->partitions
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
