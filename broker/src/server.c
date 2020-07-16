#include "server.h"

static void _esperar_cliente(int);
static void _serve_client(int);
static void _process_request(uint32_t, int);

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

    while(1) _esperar_cliente(socket_servidor);
}

/** PRIVATE FUNCTIONS **/

static void _esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);
	log_info(LOGGER, "Esperando clientes");
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	log_info(LOGGER, "Se conectó el cliente: %d", socket_cliente);

	pthread_t thread;

	pthread_create(&thread,NULL,(void*)_serve_client, socket_cliente);
	pthread_detach(thread);
}

static void _serve_client(int socket_client) {
	uint32_t cod_op;

	int result = recv(socket_client,&cod_op, sizeof(uint32_t),0);
	if(result > 0){
		_process_request(cod_op, socket_client);
	} else {
		log_info(LOGGER, "Se cayo el cliente: %d", socket);
	}
}

static void _process_request(uint32_t cod_op, int socket) {
	uint32_t generated_id = get_id();
	log_info(LOGGER, "Message id generated: %d", generated_id);
	uint32_t correlational_id = -1;
	switch (cod_op) {
	case NEW: ;
		New* new_pokemon = recv_new(socket);
		if(new_pokemon != NULL) {
			log_info(LOGGER, "Me llego un new");
			log_info(LOGGER, "Nombre pokemon: %s", new_pokemon->pokemon->name->value);
			log_info(LOGGER, "Cantidad: %d", new_pokemon->quantity);

			send(socket, &generated_id, sizeof(uint32_t), 0);

			save_message(new_pokemon, NEW, generated_id, correlational_id);

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
			int result = recv(socket,&correlational_id,sizeof(uint32_t),0);
			if(result > 0){
				log_info(LOGGER, "Me llego un caught");
				log_info(LOGGER, "Resultado: %d", caught_pokemon->result);
				log_info(LOGGER, "Id correlational: %d", correlational_id);

				send(socket, &generated_id, sizeof(uint32_t), 0);

				save_message(caught_pokemon, CAUGHT, generated_id, correlational_id);

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

			send(socket, &generated_id, sizeof(uint32_t), 0);

			save_message(get_pokemon, GET, generated_id, correlational_id);

			free_get(get_pokemon);
		} else {
			log_info(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case LOCALIZED: ;
		Localized* localized_pokemon = recv_localized(socket);

		if(localized_pokemon != NULL){
			int result = recv(socket,&correlational_id,sizeof(uint32_t),0);
			if (result > 0) {
				log_info(LOGGER, "Me llego un localized");
				log_info(LOGGER, "Nombre del pokemon: %s", localized_pokemon->pokemon->name->value);
				log_info(LOGGER, "Cantidad de coordenadas: %d", localized_pokemon->coordinates_quantity);
				for(int i = 0; i < localized_pokemon->coordinates_quantity; i++) {
					Coordinate* loc_coordinate = list_get(localized_pokemon->pokemon->coordinates, i);
					log_info(LOGGER, "Coordenada: x=%d, y=%d", loc_coordinate->pos_x, loc_coordinate->pos_y);
				}

				send(socket, &generated_id, sizeof(uint32_t), 0);

				save_message(localized_pokemon, LOCALIZED, generated_id, correlational_id);

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
			int result = recv(socket,&correlational_id,sizeof(uint32_t),0);
			if(result > 0){
				log_info(LOGGER, "Me llego un appeared");
				log_info(LOGGER, "Nombre del pokemon: %s", appeared_pokemon->name->value);
				log_info(LOGGER, "Id correlational: %d", correlational_id);
				Coordinate* coordinate = list_get(appeared_pokemon->coordinates, 0);
				log_info(LOGGER, "Coordenada: x=%d, y=%d", coordinate->pos_x, coordinate->pos_y);

				send(socket, &generated_id, sizeof(uint32_t), 0);

				save_message(appeared_pokemon, APPEARED, generated_id, correlational_id);

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

			send(socket, &generated_id, sizeof(uint32_t), 0);

			save_message(catch_pokemon, CATCH, generated_id, correlational_id);

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

		int process_id;
		result = recv(socket, &process_id, sizeof(int), MSG_WAITALL);
		if(result <= 0) {
			close(socket);
			break;
		}

		char* op = get_operation_by_value(cod_cola);
		pthread_mutex_lock(&MUTEX_SUBSCRIBERS_BY_QUEUE);
		t_list* subscribers = dictionary_get(SUBSCRIBERS_BY_QUEUE, op);
		list_add(subscribers, socket);
		pthread_mutex_unlock(&MUTEX_SUBSCRIBERS_BY_QUEUE);

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
	case -1:
		close(socket);
		break;
	}

}
