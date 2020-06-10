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

int send_caught(Caught* caught_pokemon, int socket) { //changed int y nosignal.
	int bytes = sizeof(uint32_t) * 2;
	int resultado = 0;
	void* serialized = serialize_caught(caught_pokemon, bytes);

	if (send(socket, serialized, bytes, MSG_NOSIGNAL) < 0) {
		resultado = -1;
	}

	free(serialized);
	free(caught_pokemon);
	return resultado;
}

Caught* recv_caught(int socket) { //changed
	Caught* caught_pokemon = malloc(sizeof(Caught));

	caught_pokemon->result = recv_uint32(socket);
	if (caught_pokemon->result < 0) {
		free(caught_pokemon);
		return NULL;
	}

	return caught_pokemon;
}

Caught* create_caught_pokemon(uint32_t result) {
	Caught* caught_pokemon =malloc(sizeof(Caught));

	caught_pokemon->result = result;

	return caught_pokemon;
}

