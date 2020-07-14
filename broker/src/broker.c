#include "broker.h"

typedef struct arg_struct {
	Partition* partition;
	int socket_suscriber;
} arg_struct;

static int _get_subscriber_socket(Suscriber* );
static t_list* _get_subscribers(char*);
static void* _transform_messages(Partition*, Operation);
static void send_message_and_wait_for_ack(arg_struct*);
static void send_messages_to_subscribers(t_list* subscribers, Partition* partition);

uint32_t get_id() {
	pthread_mutex_lock(&MUTEX_MESSAGE_ID);
	uint32_t generated_id = ++MESSAGE_ID;
	pthread_mutex_unlock(&MUTEX_MESSAGE_ID);
	return generated_id;
}

void consumer_new_queue() {
	sem_wait(&NEW_MESSAGES);

	t_list* new_partitions = messages_from_operation(NEW);

	void _inline_send_message(Partition* partition){
		send_messages_to_subscribers(_get_subscribers("NEW"), partition);
	}

	list_iterate(new_partitions, &_inline_send_message);
}

void consumer_appeared_queue() {
	sem_wait(&APPEARED_MESSAGES);

	t_list* appeared_partitions = messages_from_operation(APPEARED);

	void _inline_send_message(Partition* partition){
		send_messages_to_subscribers(_get_subscribers("APPEARED"), partition);
	}

	list_iterate(appeared_partitions, &_inline_send_message);

}

void consumer_localized_queue() {
	sem_wait(&LOCALIZED_MESSAGES);
	t_list* localized_partitions = messages_from_operation(LOCALIZED);

	void _inline_send_message(Partition* partition){
		send_messages_to_subscribers(_get_subscribers("LOCALIZED"), partition);
	}

	list_iterate(localized_partitions, &_inline_send_message);
}

void consumer_get_queue() {
	sem_wait(&GET_MESSAGES);

	t_list* get_partitions = messages_from_operation(GET);

	void _inline_send_message(Partition* partition){
		send_messages_to_subscribers(_get_subscribers("GET"), partition);
	}

	list_iterate(get_partitions, &_inline_send_message);

}

void consumer_catch_queue() {
	sem_wait(&CATCH_MESSAGES);

	t_list* catch_partitions = messages_from_operation(CATCH);

	void _inline_send_message(Partition* partition){
		send_messages_to_subscribers(_get_subscribers("CATCH"), partition);
	}

	list_iterate(catch_partitions, &_inline_send_message);
}

void consumer_caught_queue() {
	sem_wait(&CAUGHT_MESSAGES);

	t_list* caught_partitions = messages_from_operation(CAUGHT);

	void _inline_send_message(Partition* partition){
		send_messages_to_subscribers(_get_subscribers("CAUGHT"), partition);
	}

	list_iterate(caught_partitions, &_inline_send_message);

}

static int _get_subscriber_socket(Suscriber* suscriber) {
	return suscriber->socket;
}

static t_list* _get_subscribers(char* queue_key) {
	pthread_mutex_lock(&MUTEX_SUBSCRIBERS_BY_QUEUE);
	t_list* subscribers = list_map(dictionary_get(SUBSCRIBERS_BY_QUEUE, queue_key), &_get_subscriber_socket);
	pthread_mutex_unlock(&MUTEX_SUBSCRIBERS_BY_QUEUE);
	return subscribers;
}

static void* _transform_messages(Partition* partition, Operation operation) { //todo: esto vamos a tener que sincronizarlo porque se leen aca los datos de los start de las particiones.
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

static void send_message_and_wait_for_ack(arg_struct* args) {
	void* message = _transform_messages(args->partition, args->partition->message->operation_code);
	send(args->socket_suscriber, message, sizeof(message), MSG_NOSIGNAL);
	Result result;
	if(recv(args->socket_suscriber, &result, sizeof(Result), MSG_WAITALL) > 0){
		list_add(args->partition->notified_suscribers, args->socket_suscriber);
	}
}

static void send_messages_to_subscribers(t_list* subscribers, Partition* partition) {
	for(int i = 0; i < subscribers->elements_count; i++){
		int socket_subscriber = list_get(subscribers, i);

		arg_struct* args = malloc(sizeof(arg_struct));
		args->partition = partition;
		args->socket_suscriber = socket_subscriber;

		pthread_t thread_localized;
		pthread_create(&thread_localized, NULL, (void*)send_message_and_wait_for_ack, args);
		pthread_detach(thread_localized);
	}
}
