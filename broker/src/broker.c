#include "broker.h"

static int _get_suscriber_socket(Suscriber*);
static t_list* _get_suscribers(char*);

uint32_t get_id() {
	pthread_mutex_lock(&MUTEX_MESSAGE_ID);
	uint32_t generated_id = ++MESSAGE_ID;
	pthread_mutex_unlock(&MUTEX_MESSAGE_ID);
	return generated_id;
}

void consumer_new_queue() {
	sem_wait(&NEW_MESSAGES);

	t_list* suscribers = _get_suscribers("NEW");

	t_list* new_messages = messages_from_operation(NEW);

	void* new;
	void _send_new_to_suscribers(int socket_suscriber){
		send(socket_suscriber, new, calculate_new_bytes(), MSG_NOSIGNAL);
	}

	for(int i = 0; i < new_messages->elements_count; i++){
		new = list_get(new_messages, i);
		list_iterate(suscribers, &_send_new_to_suscribers);
	}

	free(new);
	list_destroy_and_destroy_elements(new_messages);
}

void consumer_appeared_queue() {
	sem_wait(&APPEARED_MESSAGES);
	t_list* suscribers = _get_suscribers("APPEARED");

	t_list* appeared_messages = messages_from_operation(APPEARED);

	void* appeared;
	void _send_appeared_to_suscribers(int socket_suscriber){
		send(socket_suscriber, appeared, calculate_pokemon_bytes(), MSG_NOSIGNAL);
	}

	for(int i = 0; i < appeared_messages->elements_count; i++){
		appeared = list_get(appeared_messages, i);
		list_iterate(suscribers, &_send_appeared_to_suscribers);
	}

	free(appeared);
	list_destroy_and_destroy_elements(appeared_messages);

}

void consumer_localized_queue() {
	sem_wait(&LOCALIZED_MESSAGES);
	t_list* suscribers = _get_suscribers("LOCALIZED");

	t_list* localized_messages = messages_from_operation(LOCALIZED);

	void* localized;
	void _send_localized_to_suscribers(int socket_suscriber){
		send(socket_suscriber, localized, calculate_localized_bytes(), MSG_NOSIGNAL);
	}

	for(int i = 0; i < localized_messages->elements_count; i++){
		localized = list_get(localized_messages, i);
		list_iterate(suscribers, &_send_localized_to_suscribers);
	}

	free(localized);
	list_destroy_and_destroy_elements(localized_messages);

}

void consumer_get_queue() {
	sem_wait(&GET_MESSAGES);
	t_list* suscribers = _get_suscribers("GET");

	t_list* get_messages = messages_from_operation(GET);

	void* get;
	void _send_get_to_suscribers(int socket_suscriber){
		send(socket_suscriber, get, calculate_get_bytes(), MSG_NOSIGNAL);
	}

	for(int i = 0; i < get_messages->elements_count; i++){
		get = list_get(get_messages, i);
		list_iterate(suscribers, &_send_localized_to_suscribers);
	}

	free(get);
	list_destroy_and_destroy_elements(get_messages);

}

void consumer_catch_queue() {
	sem_wait(&CATCH_MESSAGES);
	t_list* suscribers = _get_suscribers("CATCH");

	t_list* catch_messages = messages_from_operation(CATCH);

	void* catch;
	void _send_catch_to_suscribers(int socket_suscriber){
		send(socket_suscriber, catch, calculate_pokemon_bytes(), MSG_NOSIGNAL);
	}

	for(int i = 0; i < catch_messages->elements_count; i++){
		catch = list_get(catch_messages, i);
		list_iterate(suscribers, &_send_catch_to_suscribers);
	}

	free(catch);
	list_destroy_and_destroy_elements(catch_messages);
}

void consumer_caught_queue() {
	sem_wait(&CAUGHT_MESSAGES);
	t_list* suscribers = _get_suscribers("CAUGHT");

	t_list* caught_messages = messages_from_operation(CAUGHT);

	void* caught;
	void _send_caught_to_suscribers(int socket_suscriber){
		send(socket_suscriber, caught, calculate_caught_bytes(), MSG_NOSIGNAL);
	}

	for(int i = 0; i < caught_messages->elements_count; i++){
		caught = list_get(caught_messages, i);
		list_iterate(suscribers, &_send_caught_to_suscribers);
	}

	free(caught);
	list_destroy_and_destroy_elements(caught_messages);

}

static int _get_suscriber_socket(Suscriber* suscriber){
	return suscriber->socket;
}

static t_list* _get_suscribers(char* queue_key){
	pthread_mutex_lock(&MUTEX_SUBSCRIBERS_BY_QUEUE);
	t_list* suscribers = list_map(dictionary_get(SUBSCRIBERS_BY_QUEUE, queue_key), &_get_suscriber_socket);
	pthread_mutex_unlock(&MUTEX_SUBSCRIBERS_BY_QUEUE);
	return suscribers;
}

