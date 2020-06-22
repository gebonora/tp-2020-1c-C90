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
	uint32_t generated_id = get_id();
	log_info(LOGGER, "Message id generated: %d", generated_id);
	uint32_t id_correlational;
	switch (cod_op) {
	case NEW: ;
		New* new_pokemon = recv_new(socket);
		if(new_pokemon != NULL) {
			log_info(LOGGER, "Me llego un new");
			log_info(LOGGER, "Nombre pokemon: %s", new_pokemon->pokemon->name->value);
			log_info(LOGGER, "Cantidad: %d", new_pokemon->quantity);

			pthread_mutex_lock(&MUTEX_NEW_QUEUE);
			queue_push(NEW_QUEUE, new_pokemon);
			pthread_mutex_unlock(&MUTEX_NEW_QUEUE);
			sem_post(&NEW_MESSAGES);

			send(socket, &generated_id, sizeof(uint32_t), 0);
			free_new(new_pokemon);
		} else {
			log_info(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case CAUGHT: ;
		Caught* caught_pokemon = recv_caught(socket);
		 //cambio por un recv solo.

		if(caught_pokemon != NULL){
			int result = recv(socket,&id_correlational,sizeof(uint32_t),0);
			if(result > 0){
				log_info(LOGGER, "Me llego un caught");
				log_info(LOGGER, "Resultado: %d", caught_pokemon->result);
				log_info(LOGGER, "Id correlational: %d", id_correlational);

				pthread_mutex_lock(&MUTEX_CAUGHT_QUEUE);
				queue_push(CAUGHT_QUEUE, caught_pokemon);
				pthread_mutex_unlock(&MUTEX_CAUGHT_QUEUE);
				sem_post(&CAUGHT_MESSAGES);

				send(socket, &generated_id, sizeof(uint32_t), 0)
				free(caught_pokemon);
			}
		} else {
			log_info(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case GET: ;
		Get* get_pokemon = recv_get(socket);
		if(get_pokemon != NULL){
			log_info(LOGGER, "Me llego un get");
			log_info(LOGGER, "Nombre del pokemon: %s", get_pokemon->name->value);

			pthread_mutex_lock(&MUTEX_GET_QUEUE);
			queue_push(GET_QUEUE, get_pokemon);
			pthread_mutex_unlock(&MUTEX_GET_QUEUE);
			sem_post(&GET_MESSAGES);

			send(socket, &generated_id, sizeof(uint32_t), 0);
			free_get(get_pokemon);
		} else {
			log_info(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case LOCALIZED: ;
		Localized* localized_pokemon = recv_localized(socket);

		if(localized_pokemon != NULL){
			int result = recv(socket,&id_correlational,sizeof(uint32_t),0);
			if (result > 0) {
				log_info(LOGGER, "Me llego un localized");
				log_info(LOGGER, "Nombre del pokemon: %s", localized_pokemon->pokemon->name->value);
				log_info(LOGGER, "Cantidad de coordenadas: %d", localized_pokemon->coordinates_quantity);
				for(int i = 0; i < localized_pokemon->coordinates_quantity; i++) {
					Coordinate* loc_coordinate = list_get(localized_pokemon->pokemon->coordinates, i);
					log_info(LOGGER, "Coordenada: x=%d, y=%d", loc_coordinate->pos_x, loc_coordinate->pos_y);
				}

				pthread_mutex_lock(&MUTEX_LOCALIZED_QUEUE);
				queue_push(LOCALIZED_QUEUE, localized_pokemon);
				pthread_mutex_unlock(&MUTEX_LOCALIZED_QUEUE);
				sem_post(&LOCALIZED_MESSAGES);

				send(socket, &generated_id, sizeof(uint32_t), 0);
				free_localized(localized_pokemon);
			}
		} else {
			log_info(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case APPEARED: ;
		Pokemon* appeared_pokemon = recv_pokemon(socket, false);

		if(appeared_pokemon != NULL){
			int result = recv(socket,&id_correlational,sizeof(uint32_t),0);
			if(result > 0){
				log_info(LOGGER, "Me llego un appeared");
				log_info(LOGGER, "Nombre del pokemon: %s", appeared_pokemon->name->value);
				log_info(LOGGER, "Id correlational: %d", id_correlational);
				Coordinate* coordinate = list_get(appeared_pokemon->coordinates, 0);
				log_info(LOGGER, "Coordenada: x=%d, y=%d", coordinate->pos_x, coordinate->pos_y);

				pthread_mutex_lock(&MUTEX_APPEARED_QUEUE);
				queue_push(APPEARED_QUEUE, appeared_pokemon);
				pthread_mutex_unlock(&MUTEX_APPEARED_QUEUE);
				sem_post(&APPEARED_MESSAGES);

				send(socket, &generated_id, sizeof(uint32_t), 0);
				free_pokemon(appeared_pokemon);
			}
		} else {
			log_info(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case CATCH: ;
		Pokemon* catch_pokemon = recv_pokemon(socket, false);
		if(catch_pokemon != NULL) {
			log_info(LOGGER, "Me llego un catch");
			log_info(LOGGER, "Nombre del pokemon: %s", catch_pokemon->name->value);
			Coordinate* catch_coordinate = list_get(catch_pokemon->coordinates, 0);
			log_info(LOGGER, "Coordenada: x=%d, y=%d", catch_coordinate->pos_x, catch_coordinate->pos_y);

			pthread_mutex_lock(&MUTEX_CATCH_QUEUE);
			queue_push(CATCH_QUEUE, catch_pokemon);
			pthread_mutex_unlock(&MUTEX_CATCH_QUEUE);
			sem_post(&CATCH_MESSAGES);

			send(socket, &generated_id, sizeof(uint32_t), 0);
			free_pokemon(catch_pokemon);
		} else {
			log_info(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case SUBSCRIBE: ;
		log_info(LOGGER, "Me llego un suscribe");
		int result;

		int cod_process;
		result = recv(socket, &cod_process, sizeof(int), MSG_WAITALL);
		if(result <= 0) {
			close(socket);
			break;
		}

		int cod_cola;
		result = recv(socket, &cod_cola, sizeof(int), MSG_WAITALL);
		if(result <= 0) {
			close(socket);
			break;
		}

		int id_process;
		result = recv(socket, &id_process, sizeof(int), MSG_WAITALL);
		if(result <= 0) {
			close(socket);
			break;
    }

		char* op = get_operation_by_value(cod_cola);
		pthread_mutex_lock(&MUTEX_SUBSCRIBERS_BY_QUEUE);
		t_list* subscribers = dictionary_get(SUBSCRIBERS_BY_QUEUE, op);
		list_add(subscribers, socket);
		pthread_mutex_unlock(&MUTEX_SUBSCRIBERS_BY_QUEUE);

		log_info(LOGGER, "Suscripcion del proceso: %d", cod_process);
		log_info(LOGGER, "Suscripcion del proceso: %d, con id: %d", cod_process, process_id);
		log_info(LOGGER, "Suscripcion en cola: %d", cod_cola);

		// esto es de prueba nomas, cuando funcionen las queues y los envios hay que borrarlo
		while(1) {
			Result result =  OK;
			if(send(socket, &result, sizeof(Result), MSG_NOSIGNAL) < 0) {
				log_info(LOGGER, "Client is down, closing connection");
				break;
			}
			log_info(LOGGER, "Message sent");
			sleep(1);
		}

		break;
	case 0:
	case -1:
		close(socket);
		break;
	}

}
