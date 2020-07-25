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

int send_get(Get* get_pokemon, int socket) { //changed
	int bytes = calculate_get_bytes(get_pokemon);
	int resultado = 0;
	void* serialized = serialize_get(get_pokemon, bytes);

	if (send(socket, serialized, bytes, MSG_NOSIGNAL) < 0) {
		resultado = -1;
	}

	free(serialized);
	free_get(get_pokemon);
	return resultado;
}

Get* recv_get(int socket) { //changed
	Get* get_pokemon = malloc(sizeof(Get));

	get_pokemon->name = recv_name(socket);
	if (get_pokemon->name == NULL) {
		free(get_pokemon);
		return NULL;
	}

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
