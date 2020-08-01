#include "broker.h"

typedef struct arg_struct {
	Partition* partition;
	Subscriber* subscriber;
	int bytes;
	char* context;
} arg_struct;

static Message* _create_message(Operation, uint32_t, uint32_t, uint32_t);
static int _calculate_bytes_to_send(Operation, int);
static void send_messages_to_subscribers(Partition* partition);
static void send_message_and_wait_for_ack(arg_struct*);

uint32_t get_id() {
	pthread_mutex_lock(&MUTEX_MESSAGE_ID);
	uint32_t generated_id = ++MESSAGE_ID;
	pthread_mutex_unlock(&MUTEX_MESSAGE_ID);
	return generated_id;
}

void new_request(void* data, Operation operation, uint32_t message_id, uint32_t correlational_id){
	log_debug(LOGGER, "Creating message");
	Message* message = _create_message(operation, message_id, correlational_id, calculate_message_size(data, operation));
	void* serialized_data = serialize_data(data, operation, message->data_size);

	char* context = string_from_format("new_request, operation %s, message_id %d", get_operation_by_value(operation), message_id);
	lock_memory_for_write(context);
	Partition* partition = save_message(serialized_data, message);

	if(partition != NULL){
		send_messages_to_subscribers(partition);
	}

	unlock_memory_for_write(context);

	free(context);
}

void new_suscription(Operation operation, Subscriber* subscriber) {
	char* context = string_from_format("new_suscription, operation %s, subscriber %s-%d", get_operation_by_value(operation), get_process_by_value(subscriber->process), subscriber->id);
	lock_memory_for_read(context);
	log_debug(LOGGER, "Consuming messages from operation: %s", get_operation_by_value(operation));
	log_debug(LOGGER, "Getting partitions");

	t_list* partitions = messages_from_operation(operation, subscriber);
	log_debug(LOGGER, "Found %d partitions", partitions->elements_count);

	if (partitions->elements_count > 0) {
		uint32_t now = get_time();
		Partition* first_partition = partitions->head->data;
		log_debug(LOGGER, "First partition (start=%x, position=%d, size=%d, data_size=%d)", first_partition->start, first_partition->position, first_partition->size, first_partition->message->data_size);

		void _inline_send_message(void* e){
			Partition* partition = e;
			partition->access_time = now;
			arg_struct* args = malloc(sizeof(arg_struct));
			args->bytes = _calculate_bytes_to_send(operation, partition->message->data_size);
			args->partition = partition;
			args->subscriber = subscriber;
			args->context = "new_suscription";
			log_debug(LOGGER, "Args (bytes=%d, partition_start=%x, socket_subscriber: %d)", args->bytes, args->partition->start, args->subscriber->socket_subscriber);
			send_message_and_wait_for_ack(args);
		}

		list_iterate(partitions, _inline_send_message);
	}
	unlock_memory_for_read(context);
	list_destroy(partitions);
	free(context);
}

/*PRIVATE FUNCTIONS*/

static Message* _create_message(Operation operation, uint32_t message_id, uint32_t correlational_id, uint32_t data_size) {
	Message* message = malloc(sizeof(Message));
	message->operation_code = operation;
	message->message_id = message_id;
	message->correlational_id = correlational_id;
	message->data_size = data_size;
	return message;
}

static void send_messages_to_subscribers(Partition* partition) {
	bool _inline_not_notified(void* e){
		Subscriber* subscriber = e;
		return was_not_notified(partition->message->message_id, subscriber);
	}

	char* operation = get_operation_by_value(partition->message->operation_code);
	log_debug(LOGGER, "Operation: %s", operation);

	log_debug(LOGGER, "Getting subscribers from dictionary");
	t_list* subscribers = get_subscribers_by_operation(operation);

	log_debug(LOGGER, "Filtering subscribers in partition");
	t_list* filtered_subscribers = list_filter(subscribers, &_inline_not_notified);

	void _inline_send_messages(void* e){
		Subscriber* subscriber = e;
		arg_struct* args = malloc(sizeof(arg_struct));
		args->bytes = _calculate_bytes_to_send(partition->message->operation_code, partition->message->data_size);
		args->partition = partition;
		args->subscriber = subscriber;
		args->context = "new_request";

		pthread_t thread_localized;
		pthread_create(&thread_localized, NULL, (void*)send_message_and_wait_for_ack, args);
		pthread_detach(thread_localized);
	}

	pthread_t thread[list_size(filtered_subscribers)];

	for(int i = 0; i < list_size(filtered_subscribers); i++){
		Subscriber* subscriber = list_get(filtered_subscribers, i);
		arg_struct* args = malloc(sizeof(arg_struct));
		args->bytes = _calculate_bytes_to_send(partition->message->operation_code, partition->message->data_size);
		args->partition = partition;
		args->subscriber = subscriber;
		args->context = "new_request";
		pthread_create(&thread[i], NULL, (void*) send_message_and_wait_for_ack, args);
	}

	for(int i = 0; i < list_size(filtered_subscribers); i++){
		pthread_join(thread[i], NULL);
	}

	list_destroy(filtered_subscribers);
}

static int _calculate_bytes_to_send(Operation operation, int data_size) {
	int size = sizeof(Operation) + data_size;

	// a todas las operaciones menos NEW/APPEARED, le sumo el sizeof del id a enviar (message/correlational)
	switch (operation) {
	case CATCH:
	case GET:
	case LOCALIZED:
	case NEW:
	case APPEARED: ;
		size += sizeof(uint32_t) + 1;
		break;
	case CAUGHT: ;
		size += sizeof(uint32_t);
		break;
	default: ;
		size += 0;
		break;
	}

	return size;
}

static void send_message_and_wait_for_ack(arg_struct* args) {
	void* message = copy_message_from_memory(args->partition, args->partition->message->operation_code, args->bytes);

	log_info(LOGGER, "%s - message_id: %d, Adquiring mutex for subscriber with id: %d, process: %s, operation: %s", args->context, args->partition->message->message_id, args->subscriber->id, get_process_by_value(args->subscriber->process), get_operation_by_value(args->partition->message->operation_code));
	lock_subscriber(args->partition->message->operation_code, args->subscriber);
	log_info(LOGGER, "%s - message_id: %d, Mutex adquired for subscriber with id: %d, process: %s, operation: %s", args->context, args->partition->message->message_id, args->subscriber->id, get_process_by_value(args->subscriber->process), get_operation_by_value(args->partition->message->operation_code));

	log_debug(LOGGER, "%s - Sending message to: %d, with size: %d", args->context, args->subscriber->socket_subscriber, args->bytes);
	log_info(LOGGER, "%s - Enviando el mensaje %d al suscriptor con ID: %d, Proceso:  %s, Socket: %d", args->context, args->partition->message->message_id, args->subscriber->id, get_process_by_value(args->subscriber->process), args->subscriber->socket_subscriber);
	if (send(args->subscriber->socket_subscriber, message, args->bytes, MSG_NOSIGNAL) < 0) {
		log_info(LOGGER, "%s - Se cayo el suscriptor con ID: %d, Proceso:  %s, Socket: %d", args->context, args->subscriber->id, get_process_by_value(args->subscriber->process), args->subscriber->socket_subscriber);
	} else {
		Result result;
		log_debug(LOGGER, "%s - Waiting for ack", args->context);
		if(recv(args->subscriber->socket_subscriber, &result, sizeof(Result), MSG_WAITALL) > 0){
			log_info(LOGGER, "%s - ACK recibido del suscriptor %d", args->context, args->subscriber->socket_subscriber);
			log_debug(LOGGER, "%s - Adding subscriber to notified_subscribers in partition", args->context);
			add_notified_subscriber(args->partition->message->message_id, args->subscriber);
		} else {
			log_info(LOGGER, "%s - Se cayo el suscriptor con ID: %d, Proceso:  %s, Socket: %d", args->context, args->subscriber->id, get_process_by_value(args->subscriber->process), args->subscriber->socket_subscriber);
		}
	}
	log_info(LOGGER, "%s - message_id: %d, Mutex unlocked for subscriber with id: %d, process: %s, operation: %s", args->context, args->partition->message->message_id, args->subscriber->id, get_process_by_value(args->subscriber->process), get_operation_by_value(args->partition->message->operation_code));
	unlock_subscriber(args->partition->message->operation_code, args->subscriber);

	free(args);
	free(message);
}