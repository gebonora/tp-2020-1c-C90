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

int send_new(New* new_pokemon, int socket_e) { //changed. int y nosignal
	int bytes = calculate_new_bytes(new_pokemon);
	int resultado = 0;
	void* serialized = serialize_new(new_pokemon, bytes);

	if (send(socket_e, serialized, bytes, MSG_NOSIGNAL) < 0) {
		resultado = -1;
	}

	free(serialized);
	free_new(new_pokemon);
	return resultado;
}

New* recv_new(int socket_e) { //changed
	New* new_pokemon = malloc(sizeof(New));
	new_pokemon->pokemon = recv_pokemon(socket_e, false);

	if (new_pokemon->pokemon == NULL) {
		free(new_pokemon);
		return NULL;
	}

	uint32_t quantity;

	if (recv(socket_e, &quantity, sizeof(uint32_t), 0) <= 0) {
		free(new_pokemon->pokemon);
		free(new_pokemon);
		return NULL;
	}

	new_pokemon->quantity = quantity;

	return new_pokemon;
}

New* create_new_pokemon(char* name, uint32_t posx, uint32_t posy, uint32_t quantity) {
	New* new_pokemon = malloc(sizeof(New));

	new_pokemon->pokemon = create_pokemon(name, posx, posy);
	new_pokemon->quantity = quantity;

	return new_pokemon;
}
