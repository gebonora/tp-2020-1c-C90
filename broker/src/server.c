#include "server.h"

void init_server() {
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

    LOGGER = log_create("/home/utnso/tp-2020-1c-C90/broker/logs/server.log", "Broker Server", 1, LOG_LEVEL_INFO);

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1) esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);
	log_info(LOGGER, "Esperando clientes");
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	log_info(LOGGER, "Se conectó el cliente: %d", socket_cliente);

	pthread_t thread;

	pthread_create(&thread,NULL,(void*)serve_client, socket_cliente);
	pthread_detach(thread);
}

void serve_client(int socket_e) {
	int cod_op;
	recv(socket_e, &cod_op, sizeof(int), MSG_WAITALL);
	process_request(cod_op, socket_e);

}

void process_request(int cod_op, int socket) {
	int id = 900;
	uint32_t id_correlational;
	switch (cod_op) {
	case NEW: ;
		New* new_pokemon = recv_new(socket);
		log_info(LOGGER, "Me llego un new");
		log_info(LOGGER, "Nombre pokemon: %s", new_pokemon->pokemon->name->value);
		log_info(LOGGER, "Cantidad: %d", new_pokemon->quantity);
		send(socket, &id, sizeof(int), 0);
		free_new(new_pokemon);
		break;
	case CAUGHT: ;
		Caught* caught_pokemon = recv_caught(socket);
		id_correlational = recv_uint32(socket);
		log_info(LOGGER, "Me llego un caught");
		log_info(LOGGER, "Resultado: %d", caught_pokemon->result);
		log_info(LOGGER, "Id correlational: %d", id_correlational);
		send(socket, &id, sizeof(int), 0);
		free(caught_pokemon);
		break;
	case GET: ;
		Get* get_pokemon = recv_get(socket);
		log_info(LOGGER, "Me llego un get");
		log_info(LOGGER, "Nombre del pokemon: %s", get_pokemon->name->value);

 	 	pthread_mutex_lock(&MUTEX_GET_QUEUE);
		queue_push(GET_QUEUE, get_pokemon);
		pthread_mutex_unlock(&MUTEX_GET_QUEUE);
		sem_post(&GET_MESSAGES);
		Get* metido_en_cola = queue_pop(GET_QUEUE);

		log_info(LOGGER, "Saque de la cola: %s", metido_en_cola->name->value);

		send(socket, &id, sizeof(int), 0);
		free_get(get_pokemon);
		break;
	case LOCALIZED: ;
		Localized* localized_pokemon = recv_localized(socket);
		log_info(LOGGER, "Me llego un localized");
		log_info(LOGGER, "Nombre del pokemon: %s", localized_pokemon->pokemon->name->value);
		log_info(LOGGER, "Cantidad de coordenadas: %d", localized_pokemon->coordinates_quantity);
		send(socket, &id, sizeof(int), 0);
		free_localized(localized_pokemon);
		break;
	case APPEARED: ;
		Pokemon* appeared_pokemon = recv_pokemon(socket, false);
		id_correlational = recv_uint32(socket);
		log_info(LOGGER, "Me llego un appeared");
		log_info(LOGGER, "Nombre del pokemon: %s", appeared_pokemon->name->value);
		log_info(LOGGER, "Id correlational: %d", id_correlational);
		send(socket, &id, sizeof(int), 0);
		free_pokemon(appeared_pokemon);
		break;
	case CATCH: ;
		Pokemon* catch_pokemon = recv_pokemon(socket, false);
		log_info(LOGGER, "Me llego un catch");
		log_info(LOGGER, "Nombre del pokemon: %s", catch_pokemon->name->value);
		//log_info(logger, "Coordenadas: ", list_iterate()catch_pokemon->coordinates);
		send(socket, &id, sizeof(int), 0);
		free_pokemon(catch_pokemon);
		break;
	case 0:
		pthread_exit(NULL); //revisar cuando matar al hilo y cerrar la conexion
	case -1:
		pthread_exit(NULL); //revisar cuando matar al hilo y cerrar la conexion
	}

}
/*
void laburar() {
	log_info(LOGGER, "Entre a laburar");
	pthread_mutex_init(&mutex_queue, NULL);
	sem_init(&mensajes_en_queue, 0, 0);

	message_queue = queue_create();

	pthread_t producer;

	pthread_create(&producer,NULL,(void*)productor, NULL);

	pthread_t consumer;

	pthread_create(&consumer,NULL,(void*)consumidor, NULL);
	pthread_detach(producer);
	pthread_detach(consumer);
}

void productor() {
	log_info(LOGGER,"Arranca el producer");
	int valor = 0;
	while(valor != 10) {
 	 	pthread_mutex_lock(&mutex_queue);
		log_info(LOGGER,"Entra a la queue: %d", valor);
		queue_push(message_queue, valor);
		pthread_mutex_unlock(&mutex_queue);
		sem_post(&mensajes_en_queue);
		valor++;
		//sleep(1);
	}
}

void consumidor() {
	log_info(LOGGER,"Arranca el consumer");
	int mensajes_consumidos = 0;
	while(mensajes_consumidos != 10) {
		sem_wait(&mensajes_en_queue);
		pthread_mutex_lock(&mutex_queue);
		int valor = queue_pop(message_queue);
		pthread_mutex_unlock(&mutex_queue);
		log_info(LOGGER,"Sale de la queue: %d", valor);
		mensajes_consumidos++;
		//sleep(1);
	}
}
*/

