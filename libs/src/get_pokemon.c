#include "../include/interface.h"

void* serialize_get(Get* get_pokemon, int bytes) {
	int displacement = 0;
	void* serialized = malloc(bytes);

	Operation op = GET;
	memcpy(serialized + displacement, &op, sizeof(Operation));
	displacement += sizeof(Operation);

	memcpy(serialized + displacement, &(get_pokemon->name->size), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	memcpy(serialized + displacement, get_pokemon->name->value, get_pokemon->name->size);
	displacement += get_pokemon->name->size;

	return serialized;
}

int calculate_get_bytes(Get* get_pokemon) {
	return sizeof(int) + sizeof(Operation) + strlen(get_pokemon->name->value) + 1;
}

void send_get(Get* get_pokemon, int socket) {
	int bytes = calculate_get_bytes(get_pokemon);
	void* serialized = serialize_get(get_pokemon, bytes);

	send(socket, serialized, bytes, 0);

	free(serialized);
	free_get(get_pokemon);
}

Get* recv_get(int socket) {
	Get* get_pokemon = malloc(sizeof(Get));

	get_pokemon->name = recv_name(socket);

	return get_pokemon;
}

void free_get(Get* get_pokemon) {
	free_name(get_pokemon->name);
	free(get_pokemon);
}

Get* create_get_pokemon(char* name) {
	Get* get_pokemon = malloc(sizeof(Get));

	get_pokemon->name = create_name(name);

	return get_pokemon;
}
