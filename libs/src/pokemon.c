#include "../include/interface.h"

Pokemon* recv_pokemon(int socket_e, bool multiple_coordinates) { //changed
	Pokemon* pokemon = malloc(sizeof(Pokemon));
	pokemon->name = recv_name(socket_e);

	if (pokemon->name == NULL) {
		return pokemon;
		return NULL;
	}

	pokemon->coordinates = recv_coordinates(socket_e, multiple_coordinates);

	if (pokemon->coordinates == NULL) {
		free(pokemon->name);
		free(pokemon);
		return NULL;
	}
	return pokemon;
}

void free_pokemon(Pokemon* pokemon) {
	free_name(pokemon->name);
	list_destroy_and_destroy_elements(pokemon->coordinates,&free_coordinate);
	free(pokemon);
}

int calculate_pokemon_bytes(Pokemon* pokemon) {
	return sizeof(Operation) + sizeof(uint32_t) + pokemon->coordinates->elements_count * sizeof(uint32_t) * 2 + strlen(pokemon->name->value) + 1;
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

	return serialized;
}

int send_pokemon(Pokemon* pokemon, Operation operation, int socket_e) { //changed. esta si necesita ser int, usar flag MSG_NOSIGNAL!!!!!
	int bytes = calculate_pokemon_bytes(pokemon);
	int resultado = 0;
	void* serialized = serialize_pokemon(pokemon, operation, bytes);
	if (send(socket_e, serialized, bytes, MSG_NOSIGNAL) < 0) {
		resultado = -1;
	}

	free(serialized);
	free_pokemon(pokemon);
	return resultado;
}

Pokemon* create_pokemon(char* name, uint32_t posx, uint32_t posy) {

	Pokemon* pokemon = malloc(sizeof(Pokemon));

	pokemon->name = create_name(name);
	t_list* coordinates = list_create();
	Coordinate* coor = create_coordinate(posx, posy);
	list_add(coordinates, coor);
	pokemon->coordinates = coordinates;

	return pokemon;

}
