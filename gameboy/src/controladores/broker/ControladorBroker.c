//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/broker/ControladorBroker.h"
#include <readline/readline.h>
#include <stdio.h>
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"
#include <stdlib.h>
#include <netdb.h>

bool is_alive = true;

void atenderPedidoBroker(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    log_info(logger, "Se atendio el pedido en el controlador de BROKER");
    char* ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_BROKER);
    char* puerto = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_BROKER);
    int socket_broker = create_connection(ip, puerto);
    uint32_t id;
    uint32_t correlational_id;

    switch(pedidoGameBoy.tipoMensaje) {
    case NEW_POKEMON: ;
    	New* new_pokemon = create_new_pokemon(
    			list_get(pedidoGameBoy.argumentos, 0),
				atoi(list_get(pedidoGameBoy.argumentos, 1)),
				atoi(list_get(pedidoGameBoy.argumentos, 2)),
				atoi(list_get(pedidoGameBoy.argumentos, 3))
				);
		send_new(new_pokemon, socket_broker);
    break;
    case APPEARED_POKEMON: ;
		Pokemon* appeared_pokemon = create_pokemon(
				 list_get(pedidoGameBoy.argumentos, 0),
				 atoi(list_get(pedidoGameBoy.argumentos, 1)),
				 atoi(list_get(pedidoGameBoy.argumentos, 2))
				);
		correlational_id = atoi(list_get(pedidoGameBoy.argumentos, 3));
		send_pokemon(appeared_pokemon, APPEARED, socket_broker);
		send(socket_broker, &correlational_id, sizeof(uint32_t), 0);

    	recv(socket_broker, &id, sizeof(uint32_t), MSG_WAITALL);
    	log_info(logger, "Me asignaron el ID: %d", id);
    break;
    case GET_POKEMON: ;
    	Get* get_pokemon = create_get_pokemon(
    			list_get(pedidoGameBoy.argumentos, 0)
				);
    	send_get(get_pokemon, socket_broker);

    	recv(socket_broker, &id, sizeof(uint32_t), MSG_WAITALL);
    	log_info(logger, "Me asignaron el ID: %d", id);
    break;
    case LOCALIZED_POKEMON: ;
    	Name* name = create_name(list_get(pedidoGameBoy.argumentos, 0));
    	Pokemon* pokemon = malloc(sizeof(Pokemon));
    	int cant_coor = atoi(list_get(pedidoGameBoy.argumentos, 1));

    	t_list* coordinates = list_create();

    	for(int i = 0, x = 2, y = 3;  i< cant_coor; i++, x+=2, y+=2) {
    		Coordinate* coor = create_coordinate(atoi(list_get(pedidoGameBoy.argumentos, x)), atoi(list_get(pedidoGameBoy.argumentos, y)));
    		list_add(coordinates, coor);
    	}

    	Localized* localized_pokemon = malloc(sizeof(Localized));
    	localized_pokemon->coordinates_quantity = cant_coor;
    	pokemon->name = name;
    	pokemon->coordinates = coordinates;
    	localized_pokemon->pokemon = pokemon;
    	send_localized(localized_pokemon, socket_broker);

    	recv(socket_broker, &id, sizeof(uint32_t), MSG_WAITALL);
    	log_info(logger, "Me asignaron el ID: %d", id);
    break;
    case CATCH_POKEMON: ;
    	Pokemon* catch_pokemon = create_pokemon(
    				 list_get(pedidoGameBoy.argumentos, 0),
    				 atoi(list_get(pedidoGameBoy.argumentos, 1)),
    				 atoi(list_get(pedidoGameBoy.argumentos, 2))
    				);
		send_pokemon(catch_pokemon, CATCH, socket_broker);

		recv(socket_broker, &id, sizeof(uint32_t), MSG_WAITALL);
    	log_info(logger, "Me asignaron el ID: %d", id);
    break;
    case CAUGHT_POKEMON: ;
    	correlational_id = atoi(list_get(pedidoGameBoy.argumentos, 0));
    	Caught* caught_pokemon = create_caught_pokemon(
				atoi(list_get(pedidoGameBoy.argumentos, 1))
				);
		send_caught(caught_pokemon, socket_broker);
		send(socket_broker, &correlational_id, sizeof(uint32_t), 0);

		recv(socket_broker, &id, sizeof(uint32_t), MSG_WAITALL);
    	log_info(logger, "Me asignaron el ID: %d", id);
    break;
    case SUBSCRIBE_POKEMON: ;
    	log_info(logger, "ENTRE A SUBSCRIBE");
    	Operation destination_queue = get_operation(list_get(pedidoGameBoy.argumentos, 0));
    	uint32_t operation = SUBSCRIBE;
    	uint32_t process_gameboy = GAMEBOY;
    	int suscription_time = atoi(list_get(pedidoGameBoy.argumentos, 1));
    	uint32_t id_gameboy = servicioDeConfiguracion.obtenerEntero(&servicioDeConfiguracion, ID_GAMEBOY);

    	send(socket_broker, &operation, sizeof(uint32_t), 0);
    	send(socket_broker, &process_gameboy, sizeof(uint32_t), 0);
    	send(socket_broker, &destination_queue, sizeof(uint32_t), 0);
    	send(socket_broker, &id_gameboy, sizeof(uint32_t), 0);

    	socket_with_logger* swl = malloc(sizeof(socket_with_logger));
    	swl->logger = logger;
    	swl->socket = socket_broker;
    	swl->operation = destination_queue;

    	pthread_t listener_thread;
    	//pthread_create(&listener_thread,NULL,(void*)listen_messages, swl);
    	pthread_create(&listener_thread,NULL,(void*)listen_messages_test, swl);
    	pthread_detach(listener_thread);

    	log_info(logger, "Starting timer of: %d", suscription_time);
    	sleep(suscription_time);
    	is_alive = false;
    	log_info(logger, "End of timer...shutting down connection");
    	break;
    }
    close(socket_broker);
}

void listen_messages_test(socket_with_logger* swl) {
	while(is_alive) {
		Result result;
		if(recv(swl->socket, &result, sizeof(Result), MSG_WAITALL) < 0) {
			break;
		}
		log_info(swl->logger, "Message received: %d", result);
	}
}

void listen_messages(socket_with_logger* swl) {
	while(is_alive) {

		uint32_t message_id;
		uint32_t correlational_id;

		switch(swl->operation) {
		case NEW: ;
			New* new_pokemon = recv_new(swl->socket);
			log_info(swl->logger, "Message received (operation=NEW, pokemon=%s, quantity=%d)", new_pokemon->pokemon->name->value, new_pokemon->quantity);
			break;
		case APPEARED: ;
			Pokemon* appeared_pokemon = recv_pokemon(swl->socket, false);
			recv(swl->socket, &correlational_id, sizeof(uint32_t), 0);
			Coordinate* coordinate = list_get(appeared_pokemon->coordinates, 0);
			log_info(swl->logger, "Message received (operation=APPEARED, correlational_id=%d, pokemon=%s, x=%d, y=%d)", correlational_id, new_pokemon->pokemon->name->value, coordinate->pos_x, coordinate->pos_y);
			free(coordinate);
			break;
		case GET: ;
			Get* get_pokemon = recv_get(swl->socket);
			recv(swl->socket, &message_id, sizeof(uint32_t), 0);
			log_info(swl->logger, "Message received (operation=GET, message_id=%d, pokemon=%s)", message_id, get_pokemon->name);
			break;
		case LOCALIZED: ;
			Localized* localized_pokemon = recv_localized(swl->socket);
			recv(swl->socket, &correlational_id, sizeof(uint32_t), 0);
			log_info(swl->logger, "Message received (operation=LOCALIZED, correlational_id=%d, pokemon=%s, coordinates_quantity=%d)", correlational_id, localized_pokemon->pokemon->name->value, localized_pokemon->coordinates_quantity);
			for(int i = 0; i < localized_pokemon->coordinates_quantity; i++) {
				Coordinate* loc_coordinate = list_get(localized_pokemon->pokemon->coordinates, i);
				log_info(swl->logger, "Coordinate %d (x=%d, y=%d)", i, loc_coordinate->pos_x, loc_coordinate->pos_y);
				free(loc_coordinate);
			}
			break;
		case CATCH: ;
			Pokemon* catch_pokemon = recv_pokemon(swl->socket, false);
			recv(swl->socket, &message_id, sizeof(uint32_t), 0);
			Coordinate* catch_coordinate = list_get(catch_pokemon->coordinates, 0);
			log_info(swl->logger, "Message received (operation=CATCH, message_id=%d, pokemon=%s, x=%d, y=%d)", message_id, catch_pokemon->name, catch_coordinate->pos_x, catch_coordinate->pos_y);
			free(catch_coordinate);
			break;
		case CAUGHT: ;
			Caught* caught_pokemon = recv_caught(swl->socket);
			recv(swl->socket,&correlational_id,sizeof(uint32_t),0);
			log_info(swl->logger, "Message received (operation=CAUGHT, correlational_id=%d, result=%s)", correlational_id, get_result_by_value(caught_pokemon->result));
			break;
		}
	}
}

Operation get_operation(char* operation_name) {
	if (string_equals_ignore_case("NEW_POKEMON", operation_name)) {
		return NEW;
	} else if (string_equals_ignore_case("APPEARED_POKEMON", operation_name)) {
		return APPEARED;
	} else if (string_equals_ignore_case("CATCH_POKEMON", operation_name)) {
		return CATCH;
	} else if (string_equals_ignore_case("CAUGHT_POKEMON", operation_name)) {
		return CAUGHT;
	} else if (string_equals_ignore_case("GET_POKEMON", operation_name)) {
		return GET;
	} else if (string_equals_ignore_case("SUBSCRIBE_POKEMON", operation_name)) {
		return SUBSCRIBE;
	} else {
		log_error(INTERNAL_LOGGER, "Tipo de operacion no soportada: %s", operation_name);
		exit(EXIT_FAILURE);
	}
}

ControladorGameBoy controladorBroker = {.proceso=BROKER, .atenderPedido=atenderPedidoBroker};
