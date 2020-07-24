#include "../include/interface.h"

Name* recv_name(int socket_e) { //changed cuando falla una funcion que recibe, hace free -> se devuelve puntero a NULL.
	Name* name = malloc(sizeof(Name));
	uint32_t name_size;

	if (recv(socket_e, &name_size, sizeof(uint32_t), 0) <= 0) {
		free(name);
		return NULL;
	}

	char* name_value = malloc(name_size);

	if (recv(socket_e, name_value, name_size, MSG_WAITALL) <= 0) {
		free(name);
		free(name_value);
		return NULL;
	}

	name->size = name_size;
	name->value = name_value;
	return name;
}

Coordinate* recv_coordinate(int socket_e) { //changed
	Coordinate* coordinate = malloc(sizeof(Coordinate));
	uint32_t pos_x;

	if (recv(socket_e, &pos_x, sizeof(uint32_t), 0) <= 0) {
		free(coordinate);
		return NULL;
	}

	uint32_t pos_y;
	if (recv(socket_e, &pos_y, sizeof(uint32_t), 0) <= 0) {
		free(coordinate);
		return NULL;
	}

	coordinate->pos_x = pos_x;
	coordinate->pos_y = pos_y;
	return coordinate;
}

Coordinate* create_coordinate(uint32_t posx, uint32_t posy) {
	Coordinate* coor = malloc(sizeof(Coordinate));
	coor->pos_x = posx;
	coor->pos_y = posy;
	return coor;
}

t_list* recv_coordinates(int socket_e, bool multiple_coordinates) { //changed
	t_list* coordinates = list_create();
	if (multiple_coordinates) {
		uint32_t number_of_coordinates;

		if (recv(socket_e, &number_of_coordinates, sizeof(uint32_t), 0) <= 0) {
			free(coordinates);
			return NULL;
		}

		for (int i = 0; i < number_of_coordinates; i++) {
			Coordinate* coor = recv_coordinate(socket_e);

			if (coor == NULL) {
				//free elemenots dde la lista: sirve esto? NOTA: usar siempre destroy_and_elemnts??
				list_destroy_and_destroy_elements(coordinates, &free_coordinate);
				return NULL;
			}

			list_add(coordinates, coor);
		}

	} else {
		Coordinate* coor = recv_coordinate(socket_e);

		if (coor == NULL) {
			free(coordinates);
			return NULL;
		}

		list_add(coordinates, coor);
	}
	return coordinates;
}

void free_coordinate(void* coor) {
	free((Coordinate*) coor);
}

void free_name(Name* name) {
	free(name->value);
	free(name);
}

Name* create_name(char* name) {
	Name* pokemon_name = malloc(sizeof(Name));
	char* pokemon_name_value = malloc(strlen(name) + 1);
	pokemon_name_value = strcpy(pokemon_name_value, name);

	pokemon_name->value = pokemon_name_value;
	pokemon_name->size = strlen(name) + 1;

	return pokemon_name;

}

int create_connection(char *ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

char* get_process_by_value(Process value) {
	switch (value) {
	case 1:
		;
		return "GAMEBOY";
		break;
	case 2:
		;
		return "TEAM";
		break;
	case 3:
		;
		return "GAMECARD";
		break;
	case 4:
		;
		return "BROKER";
		break;
	default:
		;
		return "SUSCRIPTOR";
		break;
	}
}

char* get_operation_by_value(int value) {
	switch (value) {
	case 1:
		;
		return "NEW";
		break;
	case 2:
		;
		return "LOCALIZED";
		break;
	case 3:
		;
		return "GET";
		break;
	case 4:
		;
		return "APPEARED";
		break;
	case 5:
		;
		return "CATCH";
		break;
	case 6:
		;
		return "CAUGHT";
		break;
	default:
		return "SUBSCRIBE";
	}
}

char* get_result_by_value(uint32_t result) {
	switch(result) {
		case OK:
			return "OK";
		case FAIL:
			return "FAIL";
		default:
			return "ACKNOWLEDGE";
	}
}
