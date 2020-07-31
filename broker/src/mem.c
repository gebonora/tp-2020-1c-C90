#include "mem.h"

static Partition *_save_to_cache(void *, Message *);

/** PUBLIC FUNCTIONS **/

Partition *save_message(void *data, Message *message)
{
	if (sizeof(data) <= TAMANO_MEMORIA)
	{
		return _save_to_cache(data, message);
	}
	else
	{
		log_error(LOGGER, "Message size is bigger than memory size. Message will be not saved. Memory size: %d , Message size: %d", TAMANO_MEMORIA, sizeof(data));
		return NULL;
	}
}

t_list *messages_from_operation(Operation operation, Subscriber *subscriber)
{

	bool _inline_find_for_operation(void *e)
	{
		Partition *partition = e;
		return partition->message->operation_code == operation && was_not_notified(partition->message->message_id, subscriber);
	}

	t_list *occupied_partitions = get_occupied_partitions();

	t_list *filtered_partitions = list_filter(occupied_partitions, _inline_find_for_operation);

	bool _by_message_id(void *e1, void *e2)
	{
		Partition *partition_1 = e1;
		Partition *partition_2 = e2;

		return partition_1->message->message_id < partition_2->message->message_id;
	}

	list_sort(filtered_partitions, _by_message_id);
	list_destroy(occupied_partitions);
	return filtered_partitions;
}

void lock_memory_for_read(char *name)
{
	pthread_mutex_lock(&MEMORY_READERS_MUTEX);
	READERS++;
	if (READERS == 1){
		sem_wait(&MEMORY_WRITE_MUTEX);
	}
	int wrt_value;
	sem_getvalue(&MEMORY_WRITE_MUTEX, &wrt_value);
	log_warning(LOGGER, "lock_memory_for_read, readers count: %d, %s, writer_sem: %d", READERS, name, wrt_value);
	pthread_mutex_unlock(&MEMORY_READERS_MUTEX);
	log_debug(LOGGER, "Finish lock_memory_for_read");
}

void unlock_memory_for_read(char *name)
{
	pthread_mutex_lock(&MEMORY_READERS_MUTEX);
	READERS--;
	if (READERS == 0){
		sem_post(&MEMORY_WRITE_MUTEX);
	}
	int wrt_value;
	sem_getvalue(&MEMORY_WRITE_MUTEX, &wrt_value);
	log_warning(LOGGER, "unlock_memory_for_read, readers count: %d, %s, writer_sem: %d", READERS, name, wrt_value);
	pthread_mutex_unlock(&MEMORY_READERS_MUTEX);
	log_debug(LOGGER, "Finish unlock_memory_for_read");
}

void lock_memory_for_write(char *name)
{
	sem_wait(&MEMORY_WRITE_MUTEX);
	log_warning(LOGGER, "lock_memory_for_write: %s", name);
}

void unlock_memory_for_write(char *name)
{
	log_warning(LOGGER, "unlock_memory_for_write: %s", name);
	sem_post(&MEMORY_WRITE_MUTEX);
	log_debug(LOGGER, "Finish unlock_memory_for_write");
}

/** PRIVATE FUNCTIONS **/

static Partition *_save_to_cache(void *data, Message *message)
{
	if (string_equals_ignore_case(ALGORITMO_MEMORIA, BUDDY_SYSTEM))
	{
		log_debug(LOGGER, "Using buddy system");
		return save_to_cache_buddy_system(data, message);
	}
	else
	{
		log_debug(LOGGER, "Using dynamic partitions");
		return save_to_cache_dynamic_partitions(data, message);
	}
}
