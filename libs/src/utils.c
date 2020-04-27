#include "../include/interface.h"

uint32_t recv_uint32(int socket_e) {
	uint32_t recibido;
	recv(socket_e, &recibido, sizeof(uint32_t), 0);
	return recibido;
}

Name* recv_name(int socket_e) {
	puts("entre recv name");
	Name* name = malloc(sizeof(Name));
	uint32_t name_size = recv_uint32(socket_e);
	char* name_value = malloc(name_size);
	recv(socket_e, name_value, name_size, MSG_WAITALL);
	puts(name_value);
	name->size = name_size;
	name->value = name_value;
	puts("sali recv name");
	return name;
}

Coordinate* recv_coordinate(int socket_e) {
	puts("entre recv coor2");
	Coordinate* coordinate = malloc(sizeof(Coordinate));
	uint32_t pos_x = recv_uint32(socket_e);
	uint32_t pos_y = recv_uint32(socket_e);
	coordinate->pos_x = pos_x;
	coordinate->pos_y = pos_y;
	puts("sali recv coor2");
	return coordinate;
}

t_list* recv_coordinates(int socket_e, bool multiple_coordinates) {
	t_list* coordinates = list_create();
	puts("entre recv coor1");
	if(multiple_coordinates) {
		uint32_t number_of_coordinates = recv_uint32(socket_e);

		for(int i = 0; i < number_of_coordinates; i++) {
			list_add(coordinates, recv_coordinate(socket_e));
		}

	} else {
		Coordinate* coor = recv_coordinate(socket_e);
		list_add(coordinates, coor);
	}
	puts("sali recv coor");
	return coordinates;
}

void free_name(Name* name) {
	free(name->value);
	free(name);
}
