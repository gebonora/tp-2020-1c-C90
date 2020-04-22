#include "interface.h"

int recvUint32(int socket) {
	int recibido;
	if(recv(socket, &recibido, sizeof(int), 0)!=-1) return recibido;
	else return -1;
}
