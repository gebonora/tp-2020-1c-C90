#include "../include/interface.h"

void* serialize_caught(Caught* caught_pokemon, int bytes) {
	int displacement = 0;
	void* serialized = malloc(bytes);

	Operation op = CAUGHT;
	memcpy(serialized + displacement, &op, sizeof(Operation));
	displacement += sizeof(Operation);

	memcpy(serialized + displacement, &(caught_pokemon->result), sizeof(uint32_t));

	return serialized;
}

void send_caught(Caught* caught_pokemon, int socket) {
	int bytes = sizeof(uint32_t) * 2;
	void* serialized = serialize_caught(caught_pokemon, bytes);

	send(socket, serialized, bytes, 0);

	free(serialized);
	free(caught_pokemon);
}

Caught* recv_caught(int socket) {
	Caught* caught_pokemon = malloc(sizeof(Caught));

	caught_pokemon->result = recv_uint32(socket);

	return caught_pokemon;
}

Caught* create_caught_pokemon(uint32_t result) {
	Caught* caught_pokemon =malloc(sizeof(Caught));

	caught_pokemon->result = result;

	return caught_pokemon;
}

