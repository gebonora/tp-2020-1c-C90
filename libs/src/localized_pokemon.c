#include "../include/interface.h"

void* serialize_localized(Localized* localized_pokemon, int bytes) {
	int displacement = 0;
	void* serialized = malloc(bytes);

	Operation op = LOCALIZED;
	memcpy(serialized + displacement, &op, sizeof(Operation));
	displacement += sizeof(Operation);

	memcpy(serialized + displacement, &(localized_pokemon->pokemon->name->size), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	memcpy(serialized + displacement, localized_pokemon->pokemon->name->value, localized_pokemon->pokemon->name->size);
	displacement += localized_pokemon->pokemon->name->size;

	memcpy(serialized + displacement, &(localized_pokemon->coordinates_quantity), sizeof(uint32_t));
	displacement += sizeof(uint32_t);

	/*void _serialize_coordinate(t_link_element* element) {
		Coordinate* coordinate = (Coordinate*) element->data;
		memcpy(serialized + displacement, &(coordinate->pos_x), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		memcpy(serialized + displacement, &(coordinate->pos_y), sizeof(uint32_t));
		displacement += sizeof(uint32_t);
	}

	list_iterate(localized_pokemon->pokemon->coordinates, _serialize_coordinate);
	*/

	for(int i = 0; i < localized_pokemon->coordinates_quantity; i++) {
		Coordinate* coordinate = list_get(localized_pokemon->pokemon->coordinates, i);

		memcpy(serialized + displacement, &(coordinate->pos_x), sizeof(uint32_t));
		displacement += sizeof(uint32_t);

		memcpy(serialized + displacement, &(coordinate->pos_y), sizeof(uint32_t));
		displacement += sizeof(uint32_t);
	}

	return serialized;
}

int calculate_localized_bytes(Localized* localized_pokemon) {
	return calculate_pokemon_bytes(localized_pokemon->pokemon) + sizeof(Operation);
}

void send_localized(Localized* localized_pokemon, int socket) {
	int bytes = calculate_localized_bytes(localized_pokemon);
	void* serialized = serialize_localized(localized_pokemon, bytes);
	send(socket, serialized, bytes, 0);

	free(serialized);
	free_localized(localized_pokemon);
}

Localized* recv_localized(int socket) {
	Localized* localized_pokemon = malloc(sizeof(Localized));

	localized_pokemon->pokemon = recv_pokemon(socket, true);

	localized_pokemon->coordinates_quantity = localized_pokemon->pokemon->coordinates->elements_count;

	return localized_pokemon;
}

void free_localized(Localized* localized_pokemon) {
	free_pokemon(localized_pokemon->pokemon);
	free(localized_pokemon);
}
