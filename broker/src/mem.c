#include "mem.h"

static Partition* _save_to_cache(void*, Message*);
static bool _same_subscriber(void*, void*);

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

	bool _inline_find_for_operation(void* e){
		Partition* partition = e;
		return partition->message->operation_code == operation && not_notified(partition, subscriber);
	}

	t_list* occupied_partitions = get_occupied_partitions();

	t_list* filtered_partitions = list_filter(occupied_partitions, _inline_find_for_operation);

	bool _by_message_id(void* e1, void* e2){
		Partition* partition_1 = e1;
		Partition* partition_2 = e2;

		return partition_1->message->message_id < partition_2->message->message_id;
	}

	list_sort(filtered_partitions, _by_message_id);
	list_destroy(occupied_partitions);
	return filtered_partitions;
}

bool not_notified(void* e1, void* e2) {
	Partition* partition = e1;
	Subscriber* subscriber = e2;
	log_debug(LOGGER, "Inside not_notified");

	bool _inline_same_subscriber(void* e) {
		Subscriber* to_compare = e;
		return _same_subscriber(subscriber, to_compare);
	}

	return !list_any_satisfy(partition->notified_suscribers, _inline_same_subscriber);
}

/** PRIVATE FUNCTIONS **/

static Partition* _save_to_cache(void* data, Message* message) {
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		log_debug(LOGGER, "Using buddy system");
		return save_to_cache_buddy_system(data, message);
	} else {
		log_debug(LOGGER, "Using dynamic partitions");
		return save_to_cache_dynamic_partitions(data, message);
	}
}

static bool _same_subscriber(void* e1, void* e2) {
	Subscriber* s1 = e1;
	Subscriber* s2 = e2;
	log_debug(LOGGER, "Subscriber to compare (process=%s, id=%d), current Subscriber (process=%s, id=%d)", get_process_by_value(s1->process), s1->id, get_process_by_value(s2->process), s2->id);
	return s1->process == s2->process && s1->id == s2->id;
}
