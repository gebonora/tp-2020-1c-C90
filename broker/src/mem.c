#include "mem.h"

static Partition* _save_to_cache(void*, Message*);
static bool _not_notified(Partition*, Subscriber*);
static bool _same_subscriber(Subscriber*, Subscriber*);

/** PUBLIC FUNCTIONS **/

Partition* save_message(void* data, Message* message) {
	if(sizeof(data) <= TAMANO_MEMORIA){
		return _save_to_cache(data, message);
	} else {
		log_error(LOGGER, "Message size is bigger than memory size. Message will be not saved. Memory size: %d , Message size: %d", TAMANO_MEMORIA, sizeof(data));
		return NULL;
	}
}

t_list* messages_from_operation(Operation operation, Subscriber* subscriber){//todo sincronizar

	bool _find_for_operation(Partition* partition){
		return partition->message->operation_code == operation && _not_notified(partition, subscriber);
	}

	t_list* occupied_partitions = get_occupied_partitions();

	t_list* filtered_partitions = list_filter(occupied_partitions, &_find_for_operation);
	list_destroy(occupied_partitions);
	return filtered_partitions;
}

/** PRIVATE FUNCTIONS **/

static bool _same_subscriber(Subscriber* s1, Subscriber* s2) {
	log_debug(LOGGER, "Subscriber to compare (process=%s, id=%d), current Subscriber (process=%s, id=%d)", get_process_by_value(s1->process), s1->id, get_process_by_value(s2->process), s2->id);
	return s1->process == s2->process && s1->id == s2->id;
}

static bool _not_notified(Partition* partition, Subscriber* subscriber) {
	log_debug(LOGGER, "Inside not_notified");

	bool _inline_same_subscriber(Subscriber* to_compare) {
		return _same_subscriber(subscriber, to_compare);
	}

	return !list_any_satisfy(partition->notified_suscribers, &_inline_same_subscriber);
}

static Partition* _save_to_cache(void* data, Message* message) {
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		log_debug(LOGGER, "Using buddy system");
		return save_to_cache_buddy_system(data, message);
	} else {
		log_debug(LOGGER, "Using dynamic partitions");
		return save_to_cache_dynamic_partitions(data, message);
	}
}




