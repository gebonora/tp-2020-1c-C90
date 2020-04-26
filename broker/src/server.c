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

    logger = log_create("../logs/server.log", "Broker Server", 0, LOG_LEVEL_INFO);

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_t thread = pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);//ver como pasar el parametro socket
	pthread_detach(thread);
}

void serve_client(int* socket) {
	while (1) {
		int cod_op;

		if (recv(*socket, &cod_op, sizeof(int), 0) == -1)
			cod_op = -1;
		process_request(cod_op, *socket);
	}
}

void process_request(int cod_op, int socket) {
	switch (cod_op) {
	case NEW:
		/*
		 * recv new
		 * procesar el new: guardar en la cola/memoria, generar el id, mandar el id al que me lo envio, enviar a todos los suscriptos, etc
		 * liberar el new
		 * break*/
		New* new = recv_new(socket);
		log_info(logger, "Me llego un new");
		log_info(logger, "Nombre pokemon: %s", new->pokemon->name->value);
		free_new(new);
		break;
	case 0:
		pthread_exit(NULL); //revisar cuando matar al hilo y cerrar la conexion
	case -1:
		pthread_exit(NULL); //revisar cuando matar al hilo y cerrar la conexion
	}
}


