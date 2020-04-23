#include "interface.h"

uint32_t recv_uint32(int socket) {
	uint32_t recibido;
	if(recv(socket, &recibido, sizeof(uint32_t), 0)!=-1) return recibido;
	else return -1;
}

Name* recv_name(int socket) {
	Name* name = malloc(sizeof(Name));
	uint32_t name_size = recv_uint32(socket);
	char* name_value = malloc(name_size);
	recv(socket, name, name_size, 0);
	name->size = name_size;
	name->value = name_value;
	return name;
}

Coordinate* recv_coordinate(int socket) {
	Coordinate* coordinate = malloc(sizeof(Coordinate));
	uint32_t pos_x = recv_uint32(socket);
	uint32_t pos_y = recv_uint32(socket);
	coordinate->pos_x = pos_x;
	coordinate->pos_y = pos_y;
	return coordinate;
}
