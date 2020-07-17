#include "cliente/ClienteBroker.h"

void enviarGet(char* nombrePokemon) {
	// Dejamos como responsabilidad la liberacion de nombrePokemon afuera de esta funcion.
	int socketDescartable = crearSocketCliente(IP_Broker, Puerto_Broker);
	int flagBrokerCaido = 0;
	Get* unGet = create_get_pokemon(nombrePokemon);

	if (send_get(unGet, socketDescartable) < 0) {
		flagBrokerCaido = 1;
	}
	uint32_t idAsignado;

	if (recv(socketDescartable, &idAsignado, sizeof(uint32_t), 0) <= 0) {
		flagBrokerCaido = 1;
	}

	if (!flagBrokerCaido) {
		MensajeGet* mensajeGet = malloc(sizeof(MensajeGet));
		mensajeGet->nombrePokemon = string_duplicate(nombrePokemon);
		mensajeGet->idCorrelatividad = idAsignado;

		manejadorDeEventos.registrarGetEnEspera(&manejadorDeEventos, mensajeGet);
	} else {
		// Comportamiento caso broker caido: se asume que no hay coordenadas del pokemon pedido.
		// El unico efecto es agregar una entrada a la lista del ManejadorDeEventos. TODO: o no es necesario?
	}
}

void enviarCatch(CapturaPokemon* capturaPokemon) { //cambiar return a
	// Dejamos como responsabilidad la liberacion de capturaPokemon afuera de esta funcion.
	int socketDescartable = crearSocketCliente(IP_Broker, Puerto_Broker);
	int flagBrokerCaido = 0;
	Pokemon* unPokemon = create_pokemon(capturaPokemon->pokemonAtrapable->especie, capturaPokemon->pokemonAtrapable->posicionInicial.pos_x,
			capturaPokemon->pokemonAtrapable->posicionInicial.pos_y);
	;
	if (send_pokemon(unPokemon, socketDescartable, false) < 0) {
		flagBrokerCaido = 1;
	}
	uint32_t idAsignado;

	if (recv(socketDescartable, &idAsignado, sizeof(uint32_t), 0) <= 0) {
		flagBrokerCaido = 1;
	}

	if (!flagBrokerCaido) {
		capturaPokemon->idCorrelatividad = idAsignado;
		manejadorDeEventos.registrarCatchEnEspera(&manejadorDeEventos, capturaPokemon);
	} else {
		// Comportamiento caso broker caido: se asume que la captura fue exitosa.
		servicioDeCapturaProcesoTeam->registrarCapturaExitosa(servicioDeCapturaProcesoTeam, capturaPokemon);
	}
}
