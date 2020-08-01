#include "dump.h"

static void _create_dump();
static void _show_memory_status();
static void _show_partition(Partition*, int);
static void _show_subscriber(void*);
static void _show_message(Message*);

void dump_handler(int signum) {
	switch(signum) {
	case SIGUSR1:
		log_info(LOGGER, "Received SIGUSR1");
		_create_dump();
		log_info(LOGGER, "Dump file created at: %s", DUMP_PATH);
		break;
	case SIGUSR2:
		log_info(LOGGER, "Received SIGUSR2");
		_show_memory_status();
		break;
	default:
		log_info(LOGGER, "Received %d", signum);
		log_info(LOGGER, "Signal not recognized");
	}
}

/** PRIVATE FUNCTIONS **/

static void _create_dump() {
    FILE * dump_file;
    dump_file = fopen(DUMP_PATH, "w+");

	fprintf(dump_file, "Dump: %s\n", current_date_time_as_string());

	Partition* partition;

	// nro particion, direccion memoria comienzo, direccion fin, ocupada(X) libre(L), size,LRU, cola, id
	for(int index = 0, number = 1; index < memory->partitions->elements_count; index++, number++) {
		partition = list_get(memory->partitions, index);
		if(partition->free) {
			fprintf(dump_file, "Partición %d: | %d - %d | [%s] | Size: %d b |\n", number, partition->position, partition->position + partition->size - 1, "L", partition->size);
		} else {
			fprintf(dump_file, "Partición %d: | %d - %d | [%s] | Size: %d b | LRU: %d | COLA: %s | ID: %d |\n", number, partition->position, partition->position + partition->size - 1, "X", partition->size, partition->access_time, get_operation_by_value(partition->message->operation_code), partition->message->message_id);
		}
	}

	fclose(dump_file);
}

static void _show_memory_status() {
	log_info(LOGGER, "--------Partitions Size: %d-------", memory->partitions->elements_count);

	Partition* partition;

	// nro particion, direccion memoria comienzo, direccion fin, ocupada(X) libre(L), size,LRU, cola, id
	for(int index = 0, number = 1; index < memory->partitions->elements_count; index++, number++) {
		partition = list_get(memory->partitions, index);
		_show_partition(partition, number);
	}
}

static void _show_partition(Partition* partition, int number) {
	log_info(LOGGER, "--------------------------------");
	log_info(LOGGER, "Partition #%d", number);
	log_info(LOGGER, "Free: %s", partition->free ? "true" : "false");
	log_info(LOGGER, "Size: %d", partition->size);
	log_info(LOGGER, "Position: %d - %d", partition->position, partition->position + partition->size -1);
	log_info(LOGGER, "Start: %x (%d) - %x (%d)", partition->start, partition->start, partition->start + partition->size - 1, partition->start + partition->size - 1);
	if (string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM)) {
		log_info(LOGGER, "Buddy: %d", xor_int_and_int(partition->position, partition->size));
	}
	log_info(LOGGER, "Creation time: %d", partition->creation_time);
	log_info(LOGGER, "Last access: %d ", partition->access_time);
	if(!partition->free) {
		_show_message(partition->message);
	}
	list_iterate(get_notified_subscribers_by_message_id(partition->message->message_id), _show_subscriber);
	log_info(LOGGER, "--------------------------------");
}

static void _show_message(Message* message) {
	log_info(LOGGER, "Message Queue: %s", get_operation_by_value(message->operation_code));
	log_info(LOGGER, "Message ID: %d", message->message_id);
	if (message->correlational_id != -1) {
		log_info(LOGGER, "Correlative ID: %d", message->correlational_id);
	}
	log_info(LOGGER, "Message Size: %d", message->data_size);
}

static void _show_subscriber(void* e) {
	Subscriber* subscriber = e;
	log_info(LOGGER, "Notified Subscriber (process=%s, id=%d, socket=%d)", get_process_by_value(subscriber->process), subscriber->id, subscriber->socket_subscriber);
}