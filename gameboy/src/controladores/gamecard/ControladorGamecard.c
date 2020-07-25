
//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/gamecard/ControladorGamecard.h"

void atenderPedidoGamecard(PedidoGameBoy pedidoGameBoy, t_log * logger) {
	log_info(logger, "Se atendio el pedido en el controlador de GAMECARD");

	char* ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion,
	IP_GAMECARD);
	char* puerto = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_GAMECARD);
	int socketCliente;
	uint32_t idMensaje;

	switch (pedidoGameBoy.tipoMensaje) {
	case NEW_POKEMON:
		;
		//[POKEMON] [POSX] [POSY] [CANTIDAD] [ID_MENSAJE]
		if (list_size(pedidoGameBoy.argumentos) != 5) {
			log_error(MANDATORY_LOGGER, "Los parámetros ingresados no son compatibles con NEW_POKEMON para GAMECARD");
			break;
		}

		socketCliente = crearSocketCliente(ip, puerto);

		if (socketCliente < 0) {
			log_error(MANDATORY_LOGGER, "No se pudo enviar el mensaje.");
			break;
		}

		New* newPokemon = create_new_pokemon(list_get(pedidoGameBoy.argumentos, 0), atoi(list_get(pedidoGameBoy.argumentos, 1)),
				atoi(list_get(pedidoGameBoy.argumentos, 2)), atoi(list_get(pedidoGameBoy.argumentos, 3)));

		send_new(newPokemon, socketCliente);
		idMensaje = atoi(list_get(pedidoGameBoy.argumentos, 4));
		send(socketCliente, &idMensaje, sizeof(uint32_t), MSG_WAITALL);
		log_info(MANDATORY_LOGGER, "Se envio a GAMECARD un NEW_POKEMON, pokemon: '%s', posX: '%s', posY: '%s', cantidad: '%s', idMensaje: '%d'.",
				list_get(pedidoGameBoy.argumentos, 0), list_get(pedidoGameBoy.argumentos, 1), list_get(pedidoGameBoy.argumentos, 2),
				list_get(pedidoGameBoy.argumentos, 3), idMensaje);
		break;

	case CATCH_POKEMON:
		;
		if (list_size(pedidoGameBoy.argumentos) != 4) {
			log_error(MANDATORY_LOGGER, "Los parámetros ingresados no son compatibles con CATCH_POKEMON para GAMECARD");
			break;
		}

		socketCliente = crearSocketCliente(ip, puerto);

		if (socketCliente < 0) {
			log_error(MANDATORY_LOGGER, "No se pudo enviar el mensaje.");
			break;
		}

		//[POKEMON] [POSX] [POSY] [ID_MENSAJE]
		Pokemon* catch_pokemon = create_pokemon(list_get(pedidoGameBoy.argumentos, 0), atoi(list_get(pedidoGameBoy.argumentos, 1)),
				atoi(list_get(pedidoGameBoy.argumentos, 2)));
		send_pokemon(catch_pokemon, CATCH, socketCliente);
		idMensaje = atoi(list_get(pedidoGameBoy.argumentos, 3));
		send(socketCliente, &idMensaje, sizeof(uint32_t), MSG_WAITALL);
		log_info(MANDATORY_LOGGER, "Se envio a GAMECARD un CATCH_POKEMON, pokemon: '%s', posX: '%s', posY: '%s', idMensaje: '%d'.", list_get(pedidoGameBoy.argumentos, 0),
				list_get(pedidoGameBoy.argumentos, 1), list_get(pedidoGameBoy.argumentos, 2), idMensaje);
		break;

	case GET_POKEMON:
		;
		if (list_size(pedidoGameBoy.argumentos) != 2) {
			log_error(MANDATORY_LOGGER, "Los parámetros ingresados no son compatibles con GET_POKEMON para GAMECARD");
			break;
		}

		socketCliente = crearSocketCliente(ip, puerto);

		if (socketCliente < 0) {
			log_error(MANDATORY_LOGGER, "No se pudo enviar el mensaje.");
			break;
		}

		//[POKEMON] [ID_MENSAJE]
		Get* get_pokemon = create_get_pokemon(list_get(pedidoGameBoy.argumentos, 0));
		send_get(get_pokemon, socketCliente);
		idMensaje = atoi(list_get(pedidoGameBoy.argumentos, 1));
		send(socketCliente, &idMensaje, sizeof(uint32_t), MSG_WAITALL);
		log_info(MANDATORY_LOGGER, "Se envio a GAMECARD un GET_POKEMON, pokemon: '%s', idMensaje: '%d'.", list_get(pedidoGameBoy.argumentos, 0), idMensaje);
		break;
	default:
		log_error(MANDATORY_LOGGER, "Los parámetros ingresados no son compatibles con GAMECARD. Abortando.");
		return;
	}
}

ControladorGameBoy controladorGamecard = { .proceso = GAMECARD, .atenderPedido = atenderPedidoGamecard };
