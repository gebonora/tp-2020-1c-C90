#include "mem.h"

static void _save_to_cache(void*, Message*);
static Message* _create_message(Operation, uint32_t, uint32_t, uint32_t);
static int _calculate_data_size(void*, Operation);
static sem_t _semaphore_from_operation(Operation);

/** PUBLIC FUNCTIONS **/

void save_message(void* data, Operation operation, uint32_t message_id, uint32_t correlational_id) {
	Message* message = _create_message(operation, message_id, correlational_id, _calculate_data_size(data, operation));
	_save_to_cache(data, message);
	sem_post(&(_semaphore_from_operation(operation)));
}

/** PRIVATE FUNCTIONS **/

static void _save_to_cache(void* data, Message* message) {
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		save_to_cache_buddy_system(data, message);
	} else {
		save_to_cache_dynamic_partitions(data, message);
	}
}

t_list* messages_from_operation(Operation operation){

	bool _find_for_operation(Partition* partition){
		return partition->message->operation_code == operation;
	}

	return list_filter(memory->partitions, &_find_for_operation);
	/*t_list* messages = list_create();

	void _transform_messages(Partition* partition){
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

		list_add(messages, message);
	}

	list_iterate(partitions, &_transform_messages);

	return messages;*/

}

static int _calculate_data_size(void* data, Operation operation) {
	int size = 0;

	switch (operation) {
	case NEW: ;
		size = calculate_new_bytes(data) - sizeof(Operation);
		break;
	case APPEARED:
	case CATCH: ;
		size = calculate_pokemon_bytes(data) - sizeof(Operation);
		break;
	case GET: ;
		size = calculate_get_bytes(data) - sizeof(Operation);
		break;
	case LOCALIZED: ;
		size = calculate_localized_bytes(data) - sizeof(Operation);
		break;
	case CAUGHT: ;
		size = calculate_caught_bytes() - sizeof(Operation);
		break;
	}
	return size;
}


static Message* _create_message(Operation operation, uint32_t message_id, uint32_t correlational_id, uint32_t data_size) {
	Message* message = malloc(sizeof(Message));
	message->operation_code = operation;
	message->message_id = message_id;
	message->correlational_id = correlational_id;
	message->data_size = data_size;
	return message;
}

static sem_t _semaphore_from_operation(Operation operation){
	sem_t semaphore;
	switch (operation) {
	case NEW: ;
		semaphore = NEW_MESSAGES;
		break;
	case APPEARED:
		semaphore = APPEARED_MESSAGES;
		break;
	case CATCH: ;
		semaphore = CATCH_MESSAGES;
		break;
	case GET: ;
		semaphore = GET_MESSAGES;
		break;
	case LOCALIZED: ;
		semaphore = LOCALIZED_MESSAGES;
		break;
	case CAUGHT: ;
		semaphore = CAUGHT_MESSAGES;
		break;
	}
	return semaphore;
}
