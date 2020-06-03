//
// Created by Alan Zhao on 30/04/2020.
//

#include "controladores/gamecard/ControladorGamecard.h"

void atenderPedidoGamecard(PedidoGameBoy pedidoGameBoy, t_log * logger) {
	log_info(logger, "Se atendio el pedido en el controlador de GAMECARD");
	//TODO: Implementar atencion
	char* ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion,
	IP_GAMECARD);
	char* puerto = servicioDeConfiguracion.obtenerString(
			&servicioDeConfiguracion, PUERTO_GAMECARD);
	int socketCliente = crearSocketCliente(IP_GAMECARD, PUERTO_GAMECARD);
	uint32_t idMensaje;
	switch (pedidoGameBoy.tipoMensaje) {
	case NEW_POKEMON:
		;
		//[POKEMON] [POSX] [POSY] [CANTIDAD] [ID_MENSAJE]
		New* newPokemon = create_new_pokemon(
				list_get(pedidoGameBoy.argumentos, 0),
				atoi(list_get(pedidoGameBoy.argumentos, 1)),
				atoi(list_get(pedidoGameBoy.argumentos, 2)),
				atoi(list_get(pedidoGameBoy.argumentos, 3)));
		send_new(newPokemon, socketCliente);
		idMensaje = atoi(list_get(pedidoGameBoy.argumentos, 4));
		send(socketCliente, &idMensaje, sizeof(uint32_t), MSG_WAITALL);

		//implementar
		break;

	case CATCH_POKEMON:
		;
		//[POKEMON] [POSX] [POSY] [ID_MENSAJE]
		Pokemon* catch_pokemon = create_pokemon(
				list_get(pedidoGameBoy.argumentos, 0),
				atoi(list_get(pedidoGameBoy.argumentos, 1)),
				atoi(list_get(pedidoGameBoy.argumentos, 2)));
		send_pokemon(catch_pokemon, CATCH, socketCliente);
		idMensaje = atoi(list_get(pedidoGameBoy.argumentos, 3));
		send(socketCliente, &idMensaje, sizeof(uint32_t), MSG_WAITALL);

		break;

	case GET_POKEMON:
		;
		//[POKEMON] [ID_MENSAJE]
		Get* get_pokemon = create_get_pokemon(
				list_get(pedidoGameBoy.argumentos, 0));
		send_get(get_pokemon, socketCliente);
		idMensaje = atoi(list_get(pedidoGameBoy.argumentos, 1));
		send(socketCliente, &idMensaje, sizeof(uint32_t), MSG_WAITALL);

		break;

	}
	/* ToDo: ver el tema de mensaje id, definir si se lo manda aparte de las structs
	 * o adentro. Ver de abstraer logica para enviar los id, serian enviar un int capaz conviene usar uint32_t
	 * por el tema de eventual compatibilidad (igual que los demas enteros del protocolo).
	 */


}

ControladorGameBoy controladorGamecard = { .proceso = GAMECARD, .atenderPedido =
		atenderPedidoGamecard };
