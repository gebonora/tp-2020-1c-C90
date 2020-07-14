#include "mem.h"

static void _save_to_cache(void*, Message*);
static Message* _create_message(Operation, uint32_t, uint32_t, uint32_t);
static int _calculate_data_size(void*, Operation);
static sem_t _semaphore_from_operation(Operation);

/** PUBLIC FUNCTIONS **/

void save_message(void* data, Operation operation, uint32_t message_id, uint32_t correlational_id) {
	Message* message = _create_message(operation, message_id, correlational_id, _calculate_data_size(data, operation));
	if(sizeof(data) <= TAMANO_MEMORIA){
		_save_to_cache(data, message);
		sem_t semaphore = _semaphore_from_operation(operation);
		sem_post(&semaphore);
	} else {
		log_error(LOGGER, "Message size is bigger than memory size. Message will be not saved. Memory size: %d , Message size: %d", TAMANO_MEMORIA, sizeof(data));
	}
}

t_list* messages_from_operation(Operation operation){//todo sincronizar

	bool _find_for_operation(Partition* partition){
		return partition->message->operation_code == operation;
	}

	return list_filter(memory->partitions, &_find_for_operation);
}

/** PRIVATE FUNCTIONS **/

static void _save_to_cache(void* data, Message* message) {
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		save_to_cache_buddy_system(data, message);
	} else {
		save_to_cache_dynamic_partitions(data, message);
	}
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
