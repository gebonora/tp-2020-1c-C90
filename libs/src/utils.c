#include "../include/interface.h"

uint32_t recv_uint32(int socket_e) {
	uint32_t recibido;
	recv(socket_e, &recibido, sizeof(uint32_t), 0);
	return recibido;
}

Name* recv_name(int socket_e) {
	Name* name = malloc(sizeof(Name));
	uint32_t name_size = recv_uint32(socket_e);
	char* name_value = malloc(name_size);
	recv(socket_e, name_value, name_size, MSG_WAITALL);
	name->size = name_size;
	name->value = name_value;
	return name;
}

Coordinate* recv_coordinate(int socket_e) {
	Coordinate* coordinate = malloc(sizeof(Coordinate));
	uint32_t pos_x = recv_uint32(socket_e);
	uint32_t pos_y = recv_uint32(socket_e);
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

t_list* recv_coordinates(int socket_e, bool multiple_coordinates) {
	t_list* coordinates = list_create();
	if(multiple_coordinates) {
		uint32_t number_of_coordinates = recv_uint32(socket_e);

		for(int i = 0; i < number_of_coordinates; i++) {
			list_add(coordinates, recv_coordinate(socket_e));
		}

	} else {
		Coordinate* coor = recv_coordinate(socket_e);
		list_add(coordinates, coor);
	}
	return coordinates;
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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

char* get_operation_by_value(int value){
	switch(value){
	case 1: ;
		return "NEW";
		break;
	case 2: ;
		return "LOCALIZED";
		break;
	case 3: ;
		return "GET";
		break;
	case 4: ;
		return "APPEARED";
		break;
	case 5: ;
		return "CATCH";
		break;
	case 6: ;
		return "CAUGHT";
		break;
	default: return "SUBSCRIBE";
	}
}
