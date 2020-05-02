#include "../include/interface.h"

void* serialize_new(New* new_pokemon, int bytes) {
	int displacement = 0;
	void* serialized = malloc(bytes);

	Operation op = NEW;
	memcpy(serialized + displacement, &op, sizeof(Operation));
	displacement += sizeof(Operation);

	memcpy(serialized + displacement, &(new_pokemon->pokemon->name->size), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	memcpy(serialized + displacement, new_pokemon->pokemon->name->value, new_pokemon->pokemon->name->size);
	displacement += new_pokemon->pokemon->name->size;

	Coordinate* coordinate = (Coordinate*) new_pokemon->pokemon->coordinates->head->data;

	memcpy(serialized + displacement, &(coordinate->pos_x), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	memcpy(serialized + displacement, &(coordinate->pos_y), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	memcpy(serialized + displacement, &(new_pokemon->quantity), sizeof(uint32_t));

	return serialized;
}

void free_new(New* new_pokemon) {
	free_pokemon(new_pokemon->pokemon);
	free(new_pokemon);
}

int calculate_new_bytes(New* new_pokemon) {
	return sizeof(int) + calculate_pokemon_bytes(new_pokemon->pokemon);
}

void send_new(New* new_pokemon, int socket_e) {
	int bytes = calculate_new_bytes(new_pokemon);
	void* serialized = serialize_new(new_pokemon, bytes);

	send(socket_e, serialized, bytes, 0);

	free(serialized);
	free_new(new_pokemon);
}

New* recv_new(int socket_e) {
	New* new_pokemon = malloc(sizeof(New));
	new_pokemon->pokemon = recv_pokemon(socket_e, false);
	new_pokemon->quantity = recv_uint32(socket_e);
	return new_pokemon;
}
