#include "interface.h"

Pokemon* recv_pokemon(int socket) {
	Pokemon* pokemon = malloc(sizeof(Pokemon));

	t_list* coordinates = list_create();
	list_add(coordinates, recv_coordinate(socket));

	pokemon->name = recv_name(socket);
	pokemon->coordinates = coordinates;

	return pokemon;
}

void free_pokemon(Pokemon* pokemon) {
	free(pokemon->name->value);
	list_destroy(pokemon->coordinates);
	free(pokemon);
}

int calculate_bytes(Pokemon* pokemon) {
	return sizeof(uint32_t) * 4 + strlen(pokemon->name->value) + 1;
}

void* serialize_pokemon(Pokemon* pokemon, Operation operation, int bytes) {
	int displacement = 0;
	void* serialized = malloc(bytes);

	memcpy(serialized + displacement, &(operation), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	memcpy(serialized + displacement, &(pokemon->name->size), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	memcpy(serialized + displacement, pokemon->name->value, pokemon->name->size);
	displacement += pokemon->name->size;

	Coordinate* coordinate = (Coordinate*) pokemon->coordinates->head->data;

	memcpy(serialized + displacement, &(coordinate->pos_x), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	memcpy(serialized + displacement, &(coordinate->pos_y), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	return serialized;
}

void send_pokemon(Pokemon* pokemon, Operation operation, int socket) {
	int bytes = calculate_bytes(pokemon);
	void* serialized = serialize_pokemon(pokemon, operation, bytes);
	send(socket, serialized, bytes, 0);

	free(serialized);
	free_pokemon(pokemon);
}
