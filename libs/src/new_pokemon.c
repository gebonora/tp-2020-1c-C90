#include "../include/interface.h"

void* serialize_new(New* new_pokemon, int bytes) {
	int displacement = 0;
	void* serialized = malloc(bytes);

	memcpy(serialized + displacement, (int*) NEW, sizeof(uint32_t));
	displacement += sizeof(uint32_t);

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
	return sizeof(uint32_t) + calculate_pokemon_bytes(new_pokemon->pokemon);
}

void send_new(New* new_pokemon, int socket) {
	int bytes = calculate_new_bytes(new_pokemon);
	void* serialized = serialize_new(new_pokemon, bytes);

	send(socket, serialized, bytes, 0);

	free(serialized);
	free_new(new_pokemon);
}

New* recv_new(int socket) {
	New* new_pokemon = malloc(sizeof(New));

	new_pokemon->pokemon = recv_pokemon(socket, false);
	new_pokemon->quantity = recv_uint32(socket);

	return new_pokemon;
}
