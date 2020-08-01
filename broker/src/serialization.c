#include "serialization.h"

/** PUBLIC FUNCTIONS **/

int calculate_message_size(void* data, Operation operation) {
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

void* serialize_data(void* data, Operation operation, int bytes) {
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

void* copy_message_from_memory(Partition* partition, Operation operation, int bytes) {
	pthread_mutex_lock(&partition->mutex);
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
	pthread_mutex_unlock(&partition->mutex);
	return message;
}