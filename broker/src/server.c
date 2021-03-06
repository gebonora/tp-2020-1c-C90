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

        if(setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
        	continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

	log_info(LOGGER, "Broker server is: UP");

    while(1) _esperar_cliente(socket_servidor);
}

/** PRIVATE FUNCTIONS **/

static void _esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, (socklen_t *)&tam_direccion);

	log_info(LOGGER, "Se conecto el cliente: %d", (void* )socket_cliente);

	pthread_t thread;

	pthread_create(&thread,NULL,(void*)_serve_client, (void*)socket_cliente);
	pthread_detach(thread);
}

static void _serve_client(int socket_client) {
	uint32_t cod_op;

	int result = recv(socket_client,&cod_op, sizeof(uint32_t),0);
	if(result > 0){
		_process_request(cod_op, socket_client);
	} else {
		log_warning(LOGGER, "Se cayo el cliente: %d", socket_client);
	}
}

static void _process_request(uint32_t cod_op, int socket) {
	uint32_t generated_id;
	uint32_t correlational_id = -1;
	switch (cod_op) {
	case NEW: ;
		New* new_pokemon = recv_new(socket);
		if(new_pokemon != NULL) {
			generated_id = get_id();
			log_info(LOGGER, "Se recibio un nuevo mensaje en la cola %s. Pokemon: %s, cantidad: %d, pos_x: %d, pos_y: %d, id generado: %d, socket: %d", get_operation_by_value(cod_op),new_pokemon->pokemon->name->value, new_pokemon->quantity, ((Coordinate*)new_pokemon->pokemon->coordinates->head->data)->pos_x, ((Coordinate*)new_pokemon->pokemon->coordinates->head->data)->pos_y, generated_id, socket);
			new_request(new_pokemon, NEW, generated_id, correlational_id);
			send(socket, &generated_id, sizeof(uint32_t), 0);

			free_new(new_pokemon);
		} else {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case CAUGHT: ;
		Caught* caught_pokemon = recv_caught(socket);
		 //cambio por un recv solo.

		if(caught_pokemon != NULL){
			int result = recv(socket,&correlational_id,sizeof(uint32_t),0);
			if(result > 0){
				generated_id = get_id();
				log_info(LOGGER, "Se recibio un nuevo mensaje en la cola %s. Resultado: %s, id correlacional: %d, id generado: %d, socket: %d", get_operation_by_value(cod_op), get_result_by_value(caught_pokemon->result), correlational_id, generated_id, socket);

				new_request(caught_pokemon, CAUGHT, generated_id, correlational_id);
				send(socket, &generated_id, sizeof(uint32_t), 0);

				free(caught_pokemon);
			}
		} else {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case GET: ;
		Get* get_pokemon = recv_get(socket);
		if(get_pokemon != NULL){
			generated_id = get_id();
			log_info(LOGGER, "Se recibio un nuevo mensaje en la cola %s. Pokemon: %s, socket: %d", get_operation_by_value(cod_op), get_pokemon->name->value, socket);

			new_request(get_pokemon, GET, generated_id, correlational_id);
			send(socket, &generated_id, sizeof(uint32_t), 0);

			free_get(get_pokemon);
		} else {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case LOCALIZED: ;
		Localized* localized_pokemon = recv_localized(socket);

		if(localized_pokemon != NULL){
			int result = recv(socket,&correlational_id,sizeof(uint32_t),0);
			if (result > 0) {
				generated_id = get_id();
				log_info(LOGGER, "Se recibio un nuevo mensaje en la cola %s. Pokemon: %s, cantidad de coordenadas: %d, id correlacional: %d, id generado: %d, socket: %d", get_operation_by_value(cod_op), localized_pokemon->pokemon->name->value, localized_pokemon->coordinates_quantity, correlational_id, generated_id, socket);

				for(int i = 0; i < localized_pokemon->coordinates_quantity; i++) {
					Coordinate* loc_coordinate = list_get(localized_pokemon->pokemon->coordinates, i);
					log_info(LOGGER, "Coordenada: x=%d, y=%d, socket: %d", loc_coordinate->pos_x, loc_coordinate->pos_y, socket);
				}

				new_request(localized_pokemon, LOCALIZED, generated_id, correlational_id);
				send(socket, &generated_id, sizeof(uint32_t), 0);

				free_localized(localized_pokemon);
			} else {
				log_warning(LOGGER, "Se cayo el cliente %d", socket);
			}
		} else {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case APPEARED: ;
		Pokemon* appeared_pokemon = recv_pokemon(socket, false);

		if(appeared_pokemon != NULL){
			int result = recv(socket,&correlational_id,sizeof(uint32_t),0);
			if(result > 0){
				generated_id = get_id();
				log_info(LOGGER, "Se recibio un nuevo mensaje en la cola %s. Pokemon: %s, pos_x: %d, pos_y: %d, id correlacional: %d, id generado: %d, socket: %d", get_operation_by_value(cod_op), appeared_pokemon->name->value, ((Coordinate*)appeared_pokemon->coordinates->head->data)->pos_x, ((Coordinate*)appeared_pokemon->coordinates->head->data)->pos_y, correlational_id, generated_id, socket);

				new_request(appeared_pokemon, APPEARED, generated_id, correlational_id);
				send(socket, &generated_id, sizeof(uint32_t), 0);

				free_pokemon(appeared_pokemon);
			}
		} else {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case CATCH: ;
		Pokemon* catch_pokemon = recv_pokemon(socket, false);
		if(catch_pokemon != NULL) {
			generated_id = get_id();
			log_info(LOGGER, "Se recibio un nuevo mensaje en la cola %s. Pokemon: %s, pos_x: %d, pos_y: %d, id generado: %d, socket: %d", get_operation_by_value(cod_op), catch_pokemon->name->value, ((Coordinate*)catch_pokemon->coordinates->head->data)->pos_x, ((Coordinate*)catch_pokemon->coordinates->head->data)->pos_y, generated_id, socket);

			new_request(catch_pokemon, CATCH, generated_id, correlational_id);
			send(socket, &generated_id, sizeof(uint32_t), 0);

			free_pokemon(catch_pokemon);
		} else {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
		}
		close(socket);
		break;
	case SUBSCRIBE: ;
		int result;

		int cod_process;
		result = recv(socket, &cod_process, sizeof(int), MSG_WAITALL);
		if(result <= 0) {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
			close(socket);
			break;
		}

		int cod_cola;
		result = recv(socket, &cod_cola, sizeof(int), MSG_WAITALL);
		if(result <= 0) {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
			close(socket);
			break;
		}

		int process_id;
		result = recv(socket, &process_id, sizeof(int), MSG_WAITALL);
		if(result <= 0) {
			log_warning(LOGGER, "Se cayo el cliente: %d", socket);
			close(socket);
			break;
		}
		Result ok = OK;
		send(socket, &ok, sizeof(Result), 0);

		log_info(LOGGER, "Suscripcion proceso=%s, id=%d, cola=%s, socket=%d", get_process_by_value(cod_process), process_id, get_operation_by_value(cod_cola), socket);

		Subscriber* subscriber = create_or_update_subscriber(cod_cola, cod_process, process_id, socket);

		new_suscription(cod_cola, subscriber);

		break;
	case -1:
		close(socket);
		break;
	}

}
