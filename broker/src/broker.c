#include "broker.h"

typedef struct arg_struct {
	Partition* partition;
	Subscriber* subscriber;
	int bytes;
	char* context;
} arg_struct;

static t_list* _get_subscribers(char*);
static void* _transform_messages(Partition*, Operation, int);
static Message* _create_message(Operation, uint32_t, uint32_t, uint32_t);
static void send_message_and_wait_for_ack(arg_struct*);
static void send_message_and_wait_for_ack_2(arg_struct*);
static void send_messages_to_subscribers(Partition* partition);
static int _calculate_data_size(void*, Operation);
static void* _serialize_data(void*, Operation, int);
static int _calculate_bytes_to_send(Operation, int);
static void _lock_memory_for_read(char*);
static void _unlock_memory_for_read(char*);
static void _lock_memory_for_write(char*);
static void _unlock_memory_for_write(char*);

uint32_t get_id() {
	pthread_mutex_lock(&MUTEX_MESSAGE_ID);
	uint32_t generated_id = ++MESSAGE_ID;
	pthread_mutex_unlock(&MUTEX_MESSAGE_ID);
	return generated_id;
}

void send_message_from_new_request(void* data, Operation operation, uint32_t message_id, uint32_t correlational_id){
	log_debug(LOGGER, "Creating message");
	Message* message = _create_message(operation, message_id, correlational_id, _calculate_data_size(data, operation));
	void* serialized_data = _serialize_data(data, operation, message->data_size);

	char* context = string_from_format("send_message_from_new_request, operation %s, message_id %d", get_operation_by_value(operation), message_id);
	_lock_memory_for_write(context);
	Partition* partition = save_message(serialized_data, message);
	_unlock_memory_for_write(context);

	if(partition != NULL){
		_lock_memory_for_read(context);
		send_messages_to_subscribers(partition);
		_unlock_memory_for_read(context);
	}

	free(context);
}

void send_message_from_suscription(Operation operation, Subscriber* subscriber) {
	char* context = string_from_format("send_message_from_suscription, operation %s, subscriber %s-%d", get_operation_by_value(operation), get_process_by_value(subscriber->process), subscriber->id);
	_lock_memory_for_read(context);
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
			args->context = "NEW_SUSCRIPTION";
			log_debug(LOGGER, "Args (bytes=%d, partition_start=%x, socket_subscriber: %d)", args->bytes, args->partition->start, args->subscriber->socket_subscriber);
			send_message_and_wait_for_ack(args);
		}

		list_iterate(partitions, _inline_send_message);
	}
	_unlock_memory_for_read(context);
	list_destroy(partitions);
	free(context);
}

SubscriberWithMutex* find_mutex_by_subscriber(Operation operation, Subscriber* subscriber){
	bool _inline_find_mutex_by_subscriber(void* e){
		SubscriberWithMutex* subscriber_with_mutex = e;
		return subscriber_with_mutex->operation == operation && subscriber_with_mutex->subscriber->process == subscriber->process && subscriber_with_mutex->subscriber->id == subscriber->id;
	}

	return list_find(SUBSCRIBERS_IDENTIFIERS, _inline_find_mutex_by_subscriber);
}

/*PRIVATE FUNCTIONS*/

static void _lock_memory_for_read(char* name){
	pthread_mutex_lock(&MUTEX_READERS);
	READERS ++;
	if(READERS == 1) pthread_mutex_lock(&MEMORY);
	log_warning(LOGGER, "lock_memory_for_read, readers count: %d, %s", READERS, name);
	pthread_mutex_unlock(&MUTEX_READERS);
	log_warning(LOGGER, "lock_memory_for_read: %s", name);
}

static void _unlock_memory_for_read(char* name){
	pthread_mutex_lock(&MUTEX_READERS);
	READERS --;
	if(READERS == 0) pthread_mutex_unlock(&MEMORY);
	log_warning(LOGGER, "unlock_memory_for_read, readers count: %d, %s", READERS, name);
	pthread_mutex_unlock(&MUTEX_READERS);
	log_warning(LOGGER, "unlock_memory_for_read: %s", name);
}

static void _lock_memory_for_write(char* name){
	pthread_mutex_lock(&MEMORY);
	log_warning(LOGGER, "lock_memory_for_write: %s", name);
}

static void _unlock_memory_for_write(char* name){
	pthread_mutex_unlock(&MEMORY);
	log_warning(LOGGER, "unlock_memory_for_write: %s", name);
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

static t_list* _get_subscribers(char* queue_key) {
	pthread_mutex_lock(&MUTEX_SUBSCRIBERS_BY_QUEUE);
	t_list* subscribers = dictionary_get(SUBSCRIBERS_BY_QUEUE, queue_key);
	pthread_mutex_unlock(&MUTEX_SUBSCRIBERS_BY_QUEUE);
	return subscribers;
}

static void* _transform_messages(Partition* partition, Operation operation, int bytes) {

	void* message = malloc(bytes);
	char end_of_string = '\0';
	int displacement = 0;
	uint32_t real_size;

	log_debug(LOGGER, "Transforming messages for operation=%s, to void* of size=%d", get_operation_by_value(operation), bytes);

	switch(operation){
	case NEW: ;
		log_debug(LOGGER, "Copy from=%x, to=%x, size=%d", &operation, message, sizeof(Operation));
		memcpy(message, &operation, sizeof(Operation));
		displacement += sizeof(Operation);

		uint32_t new_name_size;
		memcpy(&new_name_size, (void*) partition->start, sizeof(uint32_t));
		log_debug(LOGGER, "New name size: %d", new_name_size);
		real_size = new_name_size +1;

		memcpy(message + displacement, &(real_size),sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t), new_name_size);
		displacement += new_name_size;
		memcpy(message + displacement, &end_of_string, 1);
		displacement += 1;
		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t) + new_name_size, partition->message->data_size - sizeof(uint32_t) - new_name_size);
		displacement += partition->message->data_size - sizeof(uint32_t) - new_name_size;
		memcpy(message + displacement, &(partition->message->message_id), sizeof(uint32_t));

		log_debug(LOGGER, "Finishing memcpy");
		break;
	case APPEARED: ;
		log_debug(LOGGER, "Copy from=%x, to=%x, size=%d", &operation, message, sizeof(Operation));
		memcpy(message, &operation, sizeof(Operation));
		displacement += sizeof(Operation);

		uint32_t appeared_name_size;
		memcpy(&appeared_name_size, (void*) partition->start, sizeof(uint32_t));
		log_debug(LOGGER, "Appeared name size: %d", appeared_name_size);
		real_size = appeared_name_size +1;

		memcpy(message + displacement, &(real_size),sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t), appeared_name_size);
		displacement += appeared_name_size;
		memcpy(message + displacement, &end_of_string, 1);
		displacement += 1;
		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t) + appeared_name_size, partition->message->data_size - sizeof(uint32_t) - appeared_name_size);
		displacement += partition->message->data_size - sizeof(uint32_t) - appeared_name_size;
		memcpy(message + displacement, &(partition->message->correlational_id), sizeof(uint32_t));

		log_debug(LOGGER, "Finishing memcpy");
		break;
	case CATCH: ;
		memcpy(message, &operation, sizeof(Operation));
		displacement += sizeof(Operation);

		uint32_t catch_name_size;
		memcpy(&catch_name_size, (void*) partition->start, sizeof(uint32_t));
		log_debug(LOGGER, "Catch name size: %d", catch_name_size);
		real_size = catch_name_size +1;

		memcpy(message + displacement, &(real_size),sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t), catch_name_size);
		displacement += catch_name_size;
		memcpy(message + displacement, &end_of_string, 1);
		displacement += 1;
		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t) + catch_name_size, partition->message->data_size - sizeof(uint32_t) - catch_name_size);
		displacement += partition->message->data_size - sizeof(uint32_t) - catch_name_size;
		memcpy(message + displacement, &(partition->message->message_id), sizeof(uint32_t));
		break;
	case CAUGHT: ;
		memcpy(message, &operation, sizeof(Operation));
		memcpy(message + sizeof(Operation), (void*) partition->start, partition->message->data_size);
		memcpy(message + sizeof(Operation) + partition->message->data_size, &(partition->message->correlational_id), sizeof(uint32_t));
		break;
	case LOCALIZED: ;
		memcpy(message, &operation, sizeof(Operation));
		displacement += sizeof(Operation);

		uint32_t localized_name_size;
		memcpy(&localized_name_size, (void*) partition->start, sizeof(uint32_t));
		log_debug(LOGGER, "Catch name size: %d", localized_name_size);
		real_size = localized_name_size +1;

		memcpy(message + displacement, &(real_size),sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t), localized_name_size);
		displacement += localized_name_size;
		memcpy(message + displacement, &end_of_string, 1);
		displacement += 1;
		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t) + localized_name_size, partition->message->data_size - sizeof(uint32_t) - localized_name_size);
		displacement += partition->message->data_size - sizeof(uint32_t) - localized_name_size;
		memcpy(message + displacement, &(partition->message->correlational_id), sizeof(uint32_t));
		break;
	case GET: ;
		memcpy(message, &operation, sizeof(Operation));
		displacement += sizeof(Operation);

		uint32_t get_name_size;
		memcpy(&get_name_size, (void*) partition->start, sizeof(uint32_t));
		log_debug(LOGGER, "Catch name size: %d", get_name_size);
		real_size = get_name_size +1;

		memcpy(message + displacement, &(real_size),sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t), get_name_size);
		displacement += get_name_size;
		memcpy(message + displacement, &end_of_string, 1);
		displacement += 1;
		memcpy(message + displacement, (void*) partition->start + sizeof(uint32_t) + get_name_size, partition->message->data_size - sizeof(uint32_t) - get_name_size);
		displacement += partition->message->data_size - sizeof(uint32_t) - get_name_size;
		memcpy(message +displacement, &(partition->message->message_id), sizeof(uint32_t));
		break;
	default: ;
		break;
	}

	log_debug(LOGGER, "Done transforming message");
	return message;
}

/*
static void send_message_and_wait_for_ack_2(arg_struct* args) {
	void* message = _transform_messages(args->partition, args->partition->message->operation_code, args->bytes);

	log_info(LOGGER, "SUBSCRIBE message_id: %d, Adquiring mutex for subscriber with id: %d, process: %s, operation: %s", args->partition->message->message_id, args->subscriber->id, get_process_by_value(args->subscriber->process), get_operation_by_value(args->partition->message->operation_code));
	SubscriberWithMutex* swm = find_mutex_by_subscriber(args->partition->message->operation_code, args->subscriber);
	pthread_mutex_lock(&swm->mutex);
	log_info(LOGGER, "SUBSCRIBE message_id: %d, Mutex adquired for subscriber with id: %d, process: %s, operation: %s", args->partition->message->message_id, swm->subscriber->id, get_process_by_value(swm->subscriber->process), get_operation_by_value(args->partition->message->operation_code));

	log_debug(LOGGER, "Sending message to: %d, with size: %d", args->subscriber->socket_subscriber, args->bytes);
	log_info(LOGGER, "Enviando el mensaje %d al suscriptor con ID: %d, Proceso:  %s, Socket: %d", args->partition->message->message_id, args->subscriber->id, get_process_by_value(args->subscriber->process), args->subscriber->socket_subscriber);
	if (send(args->subscriber->socket_subscriber, message, args->bytes, MSG_NOSIGNAL) < 0) {
		log_info(LOGGER, "Se cayo el suscriptor con ID: %d, Proceso:  %s, Socket: %d", args->subscriber->id, get_process_by_value(args->subscriber->process), args->subscriber->socket_subscriber);
	} else {
		Result result;
		log_debug(LOGGER, "Waiting for ack");
		if(recv(args->subscriber->socket_subscriber, &result, sizeof(Result), MSG_WAITALL) > 0){
			log_info(LOGGER, "ACK recibido del suscriptor %d", args->subscriber->socket_subscriber);
			log_debug(LOGGER, "Adding subscriber to notified_subscribers in partition");
			list_add(args->partition->notified_suscribers, args->subscriber);
		} else {
			log_info(LOGGER, "Se cayo el suscriptor con ID: %d, Proceso:  %s, Socket: %d", args->subscriber->id, get_process_by_value(args->subscriber->process), args->subscriber->socket_subscriber);
		}
	}
	log_info(LOGGER, "SUBSCRIBE message_id: %d, Mutex unlocked for subscriber with id: %d, process: %s, operation: %s", args->partition->message->message_id, swm->subscriber->id, get_process_by_value(swm->subscriber->process), get_operation_by_value(args->partition->message->operation_code));
	pthread_mutex_unlock(&swm->mutex);

	free(args);
	free(message);
}
*/

static void send_messages_to_subscribers(Partition* partition) {

	bool _inline_not_notified(void* e){
		Subscriber* subscriber = e;
		return not_notified(partition, subscriber);
	}

	char* operation = get_operation_by_value(partition->message->operation_code);
	log_debug(LOGGER, "Operation: %s", operation);

	log_debug(LOGGER, "Getting subscribers from dictionary");
	t_list* subscribers = _get_subscribers(operation);

	log_debug(LOGGER, "Filtering subscribers in partition");
	t_list* filtered_subscribers = list_filter(subscribers, &_inline_not_notified);

	void _inline_send_messages(void* e){
		Subscriber* subscriber = e;
		arg_struct* args = malloc(sizeof(arg_struct));
		args->bytes = _calculate_bytes_to_send(partition->message->operation_code, partition->message->data_size);
		args->partition = partition;
		args->subscriber = subscriber;
		args->context = "NEW_REQUEST";

		pthread_t thread_localized;
		pthread_create(&thread_localized, NULL, (void*)send_message_and_wait_for_ack, args);
		pthread_detach(thread_localized);
	}

	list_iterate(filtered_subscribers, &_inline_send_messages);
	list_destroy(filtered_subscribers);
}

static void send_message_and_wait_for_ack(arg_struct* args) {
	char* context = string_from_format("send_message_and_wait_for_ack, operation %s, message_id %d, subscriber %s-%d", get_operation_by_value(args->partition->message->operation_code), args->partition->message->message_id, get_process_by_value(args->subscriber->process), args->subscriber->id);

	_lock_memory_for_read(context);

	void* message = _transform_messages(args->partition, args->partition->message->operation_code, args->bytes);

	log_info(LOGGER, "%s - message_id: %d, Adquiring mutex for subscriber with id: %d, process: %s, operation: %s", args->context, args->partition->message->message_id, args->subscriber->id, get_process_by_value(args->subscriber->process), get_operation_by_value(args->partition->message->operation_code));
	SubscriberWithMutex* swm = find_mutex_by_subscriber(args->partition->message->operation_code, args->subscriber);
	pthread_mutex_lock(&swm->mutex);
	log_info(LOGGER, "%s - message_id: %d, Mutex adquired for subscriber with id: %d, process: %s, operation: %s", args->context, args->partition->message->message_id, swm->subscriber->id, get_process_by_value(swm->subscriber->process), get_operation_by_value(args->partition->message->operation_code));

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
			list_add(args->partition->notified_suscribers, args->subscriber);
		} else {
			log_info(LOGGER, "%s - Se cayo el suscriptor con ID: %d, Proceso:  %s, Socket: %d", args->context, args->subscriber->id, get_process_by_value(args->subscriber->process), args->subscriber->socket_subscriber);
		}
	}
	log_info(LOGGER, "%s - message_id: %d, Mutex unlocked for subscriber with id: %d, process: %s, operation: %s", args->context, args->partition->message->message_id, swm->subscriber->id, get_process_by_value(swm->subscriber->process), get_operation_by_value(args->partition->message->operation_code));
	pthread_mutex_unlock(&swm->mutex);

	free(args->context);
	free(args);
	free(message);

	_unlock_memory_for_read(context);

	free(context);
}

static Message* _create_message(Operation operation, uint32_t message_id, uint32_t correlational_id, uint32_t data_size) {
	Message* message = malloc(sizeof(Message));
	message->operation_code = operation;
	message->message_id = message_id;
	message->correlational_id = correlational_id;
	message->data_size = data_size;
	return message;
}

static int _calculate_data_size(void* data, Operation operation) {
	int size = 0;

	switch (operation) {
	case NEW: ;
		size = calculate_new_bytes(data) - sizeof(Operation) - 1;
		break;
	case APPEARED:
	case CATCH: ;
		size = calculate_pokemon_bytes(data) - sizeof(Operation) - 1;
		break;
	case GET: ;
		size = calculate_get_bytes(data) - sizeof(Operation) - 1;
		break;
	case LOCALIZED: ;
		size = calculate_localized_bytes(data) - sizeof(Operation) - 1;
		break;
	case CAUGHT: ;
		size = calculate_caught_bytes() - sizeof(Operation);
		break;
	default: ;
		break;
	}

	log_debug(LOGGER, "Calculated bytes: %d", size);
	return size;
}

static void* _serialize_data(void* data, Operation operation, int bytes) {
	log_debug(LOGGER, "Serializing data to save to cache, with size=%d", bytes);
	void* serialized = malloc(bytes);

	int displacement = 0;

	switch(operation){
	case NEW: ;
		New* new_pokemon = (New*) data;
		new_pokemon->pokemon->name->size -= 1;

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon name size: %d", new_pokemon->pokemon->name->size);
		memcpy(serialized, &(new_pokemon->pokemon->name->size), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon name value: %s", new_pokemon->pokemon->name->value);
		memcpy(serialized + displacement, new_pokemon->pokemon->name->value, new_pokemon->pokemon->name->size);
		displacement += new_pokemon->pokemon->name->size;

		Coordinate* coordinate = (Coordinate*) new_pokemon->pokemon->coordinates->head->data;

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon x coordinate: %d", coordinate->pos_x);
		memcpy(serialized + displacement, &(coordinate->pos_x), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon y coordinate: %d", coordinate->pos_y);
		memcpy(serialized + displacement, &(coordinate->pos_y), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon quantity: %d", new_pokemon->quantity);
		memcpy(serialized + displacement, &(new_pokemon->quantity), sizeof(uint32_t));
		break;
	case GET: ;
		Get* get_pokemon = (Get*) data;
		get_pokemon->name->size -= 1;

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon name size: %d", get_pokemon->name->size);
		memcpy(serialized, &(get_pokemon->name->size), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon name value: %s", get_pokemon->name->value);
		memcpy(serialized + displacement, get_pokemon->name->value, get_pokemon->name->size);

		break;
	case APPEARED:
	case CATCH: ;
		Pokemon* pokemon = (Pokemon*) data;
		pokemon->name->size -= 1;

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon name size: %d", pokemon->name->size);
		memcpy(serialized, &(pokemon->name->size), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon name value: %s", pokemon->name->value);
		memcpy(serialized + displacement, pokemon->name->value, pokemon->name->size);
		displacement += pokemon->name->size;

		Coordinate* pokemon_coordinate = (Coordinate*) pokemon->coordinates->head->data;

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon x coordinate: %d", pokemon_coordinate->pos_x);
		memcpy(serialized + displacement, &(pokemon_coordinate->pos_x), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon y coordinate: %d", pokemon_coordinate->pos_y);
		memcpy(serialized + displacement, &(pokemon_coordinate->pos_y), sizeof(uint32_t));

		break;
	case CAUGHT: ;
		Caught* caught_pokemon = (Caught*) data;

		log_debug(LOGGER, "Copy in %x = Start (%x) + Displacement (%d)", serialized + displacement, serialized, displacement);
		log_debug(LOGGER, "Copying pokemon name size: %d", caught_pokemon->result);
		memcpy(serialized, &(caught_pokemon->result), sizeof(uint32_t));
		break;
	case LOCALIZED: ;
		Localized* localized_pokemon = (Localized*) data;
		localized_pokemon->pokemon->name->size -= 1;

		memcpy(serialized, &(localized_pokemon->pokemon->name->size), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		memcpy(serialized + displacement, localized_pokemon->pokemon->name->value, localized_pokemon->pokemon->name->size);
		displacement += localized_pokemon->pokemon->name->size;

		memcpy(serialized + displacement, &(localized_pokemon->coordinates_quantity), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		for(int i = 0; i < localized_pokemon->coordinates_quantity; i++) {
			Coordinate* localized_coordinate = list_get(localized_pokemon->pokemon->coordinates, i);

			memcpy(serialized + displacement, &(localized_coordinate->pos_x), sizeof(uint32_t));
			displacement += sizeof(uint32_t);

			memcpy(serialized + displacement, &(localized_coordinate->pos_y), sizeof(uint32_t));
			displacement += sizeof(uint32_t);
		}

		break;
	default: ;
		break;
	}

	return serialized;
}

