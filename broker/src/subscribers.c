#include "subscribers.h"

static bool _same_subscriber(void* subscriber_to_compare, void* actual_subscriber);
static Subscriber* _find_subscriber_with_operation(Operation operation, Process process, uint32_t id);
static SubscriberWithMutex* _find_registered_subscriber(Operation operation, Subscriber* subscriber);
static Subscriber* _create_subscriber(Operation operation, Process process, uint32_t id, int socket);
static void _add_subscriber_by_operation(Operation operation, Subscriber* subscriber);
static void _register_subscriber(Operation operation, Subscriber* subscriber);

/** PUBLIC FUNCTIONS **/

t_list* get_subscribers_by_operation(char* key){
	pthread_mutex_lock(&SUBSCRIBERS_BY_OPERATION->mutex);
	t_list* subscribers = dictionary_get(SUBSCRIBERS_BY_OPERATION->map, key);
	pthread_mutex_unlock(&SUBSCRIBERS_BY_OPERATION->mutex);
	return subscribers;
}

t_list* get_notified_subscribers_by_message_id(uint32_t message_id){
    char* key = string_itoa(message_id);
	pthread_mutex_lock(&NOTIFIED_SUBSCRIBERS->mutex);
	t_list* notified_subscribers_for_id = dictionary_get(NOTIFIED_SUBSCRIBERS->map, key);
	pthread_mutex_unlock(&NOTIFIED_SUBSCRIBERS->mutex);
    free(key);
    return notified_subscribers_for_id;
}

void add_notified_subscriber(uint32_t message_id, Subscriber* subscriber){
    pthread_mutex_lock(&NOTIFIED_SUBSCRIBERS->mutex);
    char* key = string_itoa(message_id);
    t_list* notified_subscribers_for_id;

    if (dictionary_has_key(NOTIFIED_SUBSCRIBERS->map, key)){
        notified_subscribers_for_id = dictionary_get(NOTIFIED_SUBSCRIBERS->map, key);
        list_add(notified_subscribers_for_id, subscriber);
        free(key);
    } else {
        notified_subscribers_for_id = list_create();
        list_add(notified_subscribers_for_id, subscriber);
        dictionary_put(NOTIFIED_SUBSCRIBERS->map, key, notified_subscribers_for_id);
    }
    pthread_mutex_unlock(&NOTIFIED_SUBSCRIBERS->mutex);
}

bool was_notified(uint32_t message_id, void* subscriber){
    Subscriber* actual_subscriber = subscriber;
    t_list* notified_subscribers_by_id = get_notified_subscribers_by_message_id(message_id);

    if(notified_subscribers_by_id != NULL){
        bool _inline_same_subscriber(void* e) {
            Subscriber* to_compare = e;
            return _same_subscriber(actual_subscriber, to_compare);
        }
	    return list_any_satisfy(notified_subscribers_by_id, _inline_same_subscriber);
    }

    return false;
}

bool was_not_notified(uint32_t message_id, void* subscriber){
    return !was_notified(message_id, subscriber);
}

Subscriber* create_or_update_subscriber(Operation operation, Process process, uint32_t id, int socket){
    Subscriber* subscriber = _find_subscriber_with_operation(operation, process, id);

    if (subscriber != NULL) {
        log_debug(LOGGER, "Subscriber already in list, updating socket from=%d, to=%d", subscriber->socket_subscriber, socket);
        subscriber->socket_subscriber = socket;
    } else {
        log_debug(LOGGER, "Subscriber not present in list, creating new one");
        subscriber = _create_subscriber(operation, process, id, socket);
    }

    return subscriber;
}

void lock_subscriber(Operation operation, Subscriber* subscriber){
    SubscriberWithMutex* registered_subscriber = _find_registered_subscriber(operation, subscriber);

    if (registered_subscriber != NULL){
        pthread_mutex_lock(&registered_subscriber->mutex);
    }
}

void unlock_subscriber(Operation operation, Subscriber* subscriber){
    SubscriberWithMutex* registered_subscriber = _find_registered_subscriber(operation, subscriber);

    if (registered_subscriber != NULL){
        pthread_mutex_unlock(&registered_subscriber->mutex);
    }
}

/** PRIVATE FUNCTIONS **/

static bool _same_subscriber(void* e1, void* e2) {
	Subscriber* first_subscriber = e1;
	Subscriber* second_subscriber = e2;
	log_debug(LOGGER, "Subscriber to compare (process=%s, id=%d), current Subscriber (process=%s, id=%d)", get_process_by_value(first_subscriber->process), first_subscriber->id, get_process_by_value(second_subscriber->process), second_subscriber->id);
	return first_subscriber->process == second_subscriber->process && first_subscriber->id == second_subscriber->id;
}

static SubscriberWithMutex* _find_registered_subscriber(Operation operation, Subscriber* subscriber){
	bool _inline_find_registered_subscriber(void* e){
		SubscriberWithMutex* subscriber_with_mutex = e;
		return subscriber_with_mutex->operation == operation && subscriber_with_mutex->subscriber->process == subscriber->process && subscriber_with_mutex->subscriber->id == subscriber->id;
	}

    pthread_mutex_lock(&REGISTERED_SUBSCRIBERS->mutex);
	SubscriberWithMutex* registered_subscriber = list_find(REGISTERED_SUBSCRIBERS->list, _inline_find_registered_subscriber);
    pthread_mutex_unlock(&REGISTERED_SUBSCRIBERS->mutex);
    return registered_subscriber;
}

static Subscriber* _find_subscriber_with_operation(Operation operation, Process process, uint32_t id){
    bool _inline_find_subscriber_with_operation(void* e) {
        Subscriber* to_compare = e;
        return process == to_compare->process && id == to_compare->id;
    }

    pthread_mutex_lock(&SUBSCRIBERS_BY_OPERATION->mutex);
    t_list* subscribers_by_operation = dictionary_get(SUBSCRIBERS_BY_OPERATION->map, get_operation_by_value(operation));
    Subscriber* subscriber = list_find(subscribers_by_operation, _inline_find_subscriber_with_operation);
    pthread_mutex_unlock(&SUBSCRIBERS_BY_OPERATION->mutex);
    return subscriber;
}

static Subscriber* _create_subscriber(Operation operation, Process process, uint32_t id, int socket){
    Subscriber* subscriber = malloc(sizeof(Subscriber));
    subscriber->process = process;
    subscriber->id = id;
    subscriber->socket_subscriber = socket;

    _add_subscriber_by_operation(operation, subscriber);
    _register_subscriber(operation, subscriber);

    return subscriber;
}

static void _add_subscriber_by_operation(Operation operation, Subscriber* subscriber){
    pthread_mutex_lock(&SUBSCRIBERS_BY_OPERATION->mutex);
    t_list* subscribers = dictionary_get(SUBSCRIBERS_BY_OPERATION->map, get_operation_by_value(operation));
    list_add(subscribers, subscriber);
    pthread_mutex_unlock(&SUBSCRIBERS_BY_OPERATION->mutex);
}

static void _register_subscriber(Operation operation, Subscriber* subscriber){
    SubscriberWithMutex* subscriber_with_mutex = malloc(sizeof(SubscriberWithMutex));
    subscriber_with_mutex->operation = operation;
    subscriber_with_mutex->subscriber = subscriber;
    
    pthread_mutex_init(&subscriber_with_mutex->mutex, NULL);
    pthread_mutex_lock(&REGISTERED_SUBSCRIBERS->mutex);
    list_add(REGISTERED_SUBSCRIBERS->list, subscriber_with_mutex);
    pthread_mutex_unlock(&REGISTERED_SUBSCRIBERS->mutex);
}