#include "interface.h"

Catch* recvCatch(int socket) {
	Catch* catch = malloc(sizeof(Catch));
	uint32_t sizeName = recvUint32(socket);
	char* name = malloc(sizeName);
	recv(socket, name, sizeName, 0);
	uint32_t posX = recvUint32(socket);
	uint32_t posY = recvUint32(socket);

	catch->sizeName = sizeName;
	catch->pokemonName = name;
	catch->posX = posX;
	catch->posY = posY;

	return catch;
}

void freeCatch(Catch* catch) {
	free(catch->pokemonName);
	free(catch);
}

int calcularBytesCatch(Catch* messageCatch) {
	return sizeof(uint32_t) * 4 + strlen(messageCatch->pokemonName) + 1;
}

void* serializarCatch(Message* messageCatch, int bytes) {
	int desplazamiento = 0;
	void* serializado = malloc(bytes);

	memcpy(serializado + desplazamiento, &(messageCatch->operationCode), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(serializado + desplazamiento, &((Catch*)(messageCatch->buffer))->sizeName, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(serializado + desplazamiento, ((Catch*)(messageCatch->buffer))->pokemonName, ((Catch*)(messageCatch->buffer))->sizeName);
	desplazamiento += ((Catch*)(messageCatch->buffer))->sizeName;

	memcpy(serializado + desplazamiento, &((Catch*)(messageCatch->buffer))->posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(serializado + desplazamiento, &((Catch*)(messageCatch->buffer))->posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return serializado;
}

void sendCatch(Catch* catch, int socket) {
	Message* messageCatch = malloc(sizeof(Message));
	messageCatch->operationCode = CATCH;
	messageCatch->buffer = catch;

	int bytes = calcularBytesCatch(catch);
	void* serializado = serializarCatch(messageCatch, bytes);

	send(socket, serializado, bytes, 0);

	free(serializado);
	freeCatch(catch);
	free(messageCatch);

}
