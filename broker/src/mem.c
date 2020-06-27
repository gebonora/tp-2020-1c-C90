#include "mem.h"

static void _save_to_cache(void*, Message*);
static Message* _create_message(Operation, uint32_t, uint32_t, uint32_t);
static int _calculate_data_size(void*, Operation);

/** PUBLIC FUNCTIONS **/

void save_message(void* data, Operation operation, uint32_t message_id, uint32_t correlational_id) {
	Message* message = _create_message(operation, message_id, correlational_id, _calculate_data_size(data, operation));
	_save_to_cache(data, message);
}

/** PRIVATE FUNCTIONS **/

static void _save_to_cache(void* data, Message* message) {
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		save_to_cache_buddy_system(data, message);
	} else {
		save_to_cache_dynamic_partitions(data, message);
	}

	// TODO: luego de esto deberia avisarle al sem de la operacion que hay un data nuevo para consumir
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
		size = calculate_new_bytes(data);
		break;
	case APPEARED:
	case CATCH: ;
		size = calculate_pokemon_bytes(data);
		break;
	case GET: ;
		size = calculate_get_bytes(data);
		break;
	case LOCALIZED: ;
		size = calculate_localized_bytes(data);
		break;
	case CAUGHT: ;
		size = calculate_caught_bytes();
		break;
	}
	return size;
}
