#include "server.h"

void iniciar_servidor(void) {
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, PUERTO, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

    logger = log_create("/home/utnso/tp-2020-1c-C90/broker/logs/server.log", "Broker Server", 1, LOG_LEVEL_INFO);

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	serve_client(socket_cliente);

	pthread_t thread;

	pthread_create(&thread,NULL,(void*)serve_client, &socket_cliente);//ver como pasar el parametro socket
	pthread_detach(thread);
}

void serve_client(int socket_e) {
	int cod_op;
	recv(socket_e, &cod_op, sizeof(int), MSG_WAITALL);
	process_request(cod_op, socket_e);

}

void process_request(int cod_op, int socket) {
	switch (cod_op) {
	case NEW: ;
		New* new_pokemon = recv_new(socket);
		log_info(logger, "Me llego un new");
		log_info(logger, "Nombre pokemon: %s", new_pokemon->pokemon->name->value);
		int id = 900;
		send(socket, &id, sizeof(int), 0);
		//free_new(new_pokemon);
		break;
	case CAUGHT: ;
		Caught* caught_pokemon = recv_caught(socket);
		log_info(logger, "Me llego un caught");
		log_info(logger, "Resultado: %d", caught_pokemon->result);
		int id2 = 900;
		send(socket, &id2, sizeof(int), 0);
		break;
	case 0:
		pthread_exit(NULL); //revisar cuando matar al hilo y cerrar la conexion
	case -1:
		pthread_exit(NULL); //revisar cuando matar al hilo y cerrar la conexion
	}
}


