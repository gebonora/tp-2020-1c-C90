#include "broker.h"

static int _get_suscriber_socket(Suscriber*);
static t_list* _get_suscribers(char*);

struct arg_struct{
	Partition* partition;
	int socket_suscriber;
};

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

	t_list* localized_partirions_messages = messages_from_operation(LOCALIZED);

	void* localized;
	void _send_localized_to_suscribers(int socket_suscriber){
		send(socket_suscriber, localized, calculate_localized_bytes(), MSG_NOSIGNAL);
	}

	void hola(Partition* partition){
		void* message = _transform_messages(partition, LOCALIZED);
		pthread_t thread_localized;
		pthread_create(&thread_localized, NULL,(void*)_send_localized_to_suscribers, NULL);
		pthread_detach(thread_localized);
	}

	list_iterate(localized_partirions_messages, &hola);


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

void wait_for_ack(int socket_suscriber){
	Result result;
	if(recv(socket_suscriber, &result, sizeof(Result), MSG_WAITALL) != -1){

	}
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

void* _transform_messages(Partition* partition, Operation, operation){
	int now = (int) ahoraEnTimeT();
	partition->access_time = now;
	void* message;

	switch(operation){
	case NEW: ;
		message = malloc(sizeof(Operation) + partition->message->data_size);
		memcpy(message, &operation, sizeof(Operation));
		memcpy(message + sizeof(Operation), partition->start, partition->message->data_size);
		break;
	case APPEARED: ;
		message = malloc(sizeof(Operation) + partition->message->data_size);
		memcpy(message, &operation, sizeof(Operation));
		memcpy(message + sizeof(Operation), partition->start, partition->message->data_size);
		break;
	case CATCH: ;
		message = malloc(sizeof(Operation) + partition->message->data_size + sizeof(uint32_t));
		memcpy(message, &operation, sizeof(Operation));
		memcpy(message + sizeof(Operation), partition->start, partition->message->data_size);
		memcpy(message + sizeof(Operation) + sizeof(uint32_t), &(partition->message->message_id), sizeof(uint32_t));
		break;
	case CAUGHT: ;
		message = malloc(sizeof(Operation) + partition->message->data_size + sizeof(uint32_t));
		memcpy(message, &operation, sizeof(Operation));
		memcpy(message + sizeof(Operation), partition->start, partition->message->data_size);
		memcpy(message + sizeof(Operation) + sizeof(uint32_t), &(partition->message->correlational_id), sizeof(uint32_t));
		break;
	case LOCALIZED: ;
		message = malloc(sizeof(Operation) + partition->message->data_size + sizeof(uint32_t));
		memcpy(message, &operation, sizeof(Operation));
		memcpy(message + sizeof(Operation), partition->start, partition->message->data_size);
		memcpy(message + sizeof(Operation) + sizeof(uint32_t), &(partition->message->correlational_id), sizeof(uint32_t));
		break;
	case GET: ;
		message = malloc(sizeof(Operation) + partition->message->data_size + sizeof(uint32_t));
		memcpy(message, &operation, sizeof(Operation));
		memcpy(message + sizeof(Operation), partition->start, partition->message->data_size);
		memcpy(message + sizeof(Operation) + sizeof(uint32_t), &(partition->message->message_id), sizeof(uint32_t));
		break;
	}

	return message;
}
