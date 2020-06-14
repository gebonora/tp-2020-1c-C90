//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/broker/ControladorBroker.h"
#include <readline/readline.h>
#include <stdio.h>
#include "support/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"
#include <stdlib.h>
#include <netdb.h>

bool is_alive = true;

void atenderPedidoBroker(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    log_info(logger, "Se atendio el pedido en el controlador de BROKER");
    char* ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_BROKER);
    char* puerto = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_BROKER);
    int socket_broker = create_connection(ip, puerto);
    int id;
    uint32_t id_correlational;

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
		id_correlational = atoi(list_get(pedidoGameBoy.argumentos, 3));
		send_pokemon(appeared_pokemon, APPEARED, socket_broker);
		send(socket_broker, &id_correlational, sizeof(uint32_t), 0);
    break;
    case CATCH_POKEMON: ;
    	Pokemon* catch_pokemon = create_pokemon(
    				 list_get(pedidoGameBoy.argumentos, 0),
    				 atoi(list_get(pedidoGameBoy.argumentos, 1)),
    				 atoi(list_get(pedidoGameBoy.argumentos, 2))
    				);
		send_pokemon(catch_pokemon, CATCH, socket_broker);
    break;
    case CAUGHT_POKEMON: ;
    	id_correlational = atoi(list_get(pedidoGameBoy.argumentos, 0));
    	Caught* caught_pokemon = create_caught_pokemon(
				atoi(list_get(pedidoGameBoy.argumentos, 1))
				);
		send_caught(caught_pokemon, socket_broker);
		send(socket_broker, &id_correlational, sizeof(uint32_t), 0);
		recv(socket_broker, &id, sizeof(int), MSG_WAITALL);
    break;
    case GET_POKEMON: ;
    	Get* get_pokemon = create_get_pokemon(
    			list_get(pedidoGameBoy.argumentos, 0)
				);
    	send_get(get_pokemon, socket_broker);
    	recv(socket_broker, &id, sizeof(int), MSG_WAITALL);
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
    	recv(socket_broker, &id, sizeof(int), MSG_WAITALL);

    break;
    case SUBSCRIBE: ;
    	log_info(logger, "ENTRE A SUBSCRIBE");
    	Operation destination_queue = get_operation(list_get(pedidoGameBoy.argumentos, 0));
    	uint32_t operation = SUBSCRIBE;
    	uint32_t process_gameboy = GAMEBOY;
    	int suscription_time = atoi(list_get(pedidoGameBoy.argumentos, 1));
    	send(socket_broker, &operation, sizeof(uint32_t), 0);
    	send(socket_broker, &process_gameboy, sizeof(uint32_t), 0);
    	send(socket_broker, &destination_queue, sizeof(uint32_t), 0);

    	socket_with_logger* swl = malloc(sizeof(socket_with_logger));
    	swl->logger = logger;
    	swl->socket = socket_broker;

    	pthread_t listener_thread;
    	// este listen_messages tiene qu recibir el tipo de cola asi puedo leer el mensaje
    	// que me llega correctamente en un switch
    	pthread_create(&listener_thread,NULL,(void*)listen_messages, swl);
    	pthread_detach(listener_thread);

    	log_info(logger, "Starting timer of: %d", suscription_time);
    	sleep(suscription_time);
    	is_alive = false;
    	log_info(logger, "End of timer...shutting down connection");
    	break;
    }
    close(socket_broker);
}

void listen_messages(socket_with_logger* swl) {
	// crear un switch para atender el tipo de mensaje puntual
	while(is_alive) {
		Result result;
		if(recv(swl->socket, &result, sizeof(Result), MSG_WAITALL) < 0) {
			break;
		}
		log_info(swl->logger, "New message: %d", result);
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
	} else {
		log_error(INTERNAL_LOGGER, "Tipo de operacion no soportada: %s", operation_name);
		exit(EXIT_FAILURE);
	}
}

ControladorGameBoy controladorBroker = {.proceso=BROKER, .atenderPedido=atenderPedidoBroker};
