#include "cliente/ClienteBroker.h"

void enviarGet(char* nombrePokemon) {
	//dejamos como responsabilidad la liberacion de nombrePokemon afuera de esta funcion.
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

	if (flagBrokerCaido) {
		manejadorDeEventos.registrarMensajeEnEsperaEnLista(&manejadorDeEventos, nombrePokemon, idAsignado, (&manejadorDeEventos)->listaGetEnEspera);
	} else {
		//comportamiento caso broker caido
	}
}

void enviarCatch(char* nombrePokemon, uint32_t posX, uint32_t posY) {
	//dejamos como responsabilidad la liberacion de nombrePokemon afuera de esta funcion.
	int socketDescartable = crearSocketCliente(IP_Broker, Puerto_Broker);
	int flagBrokerCaido = 0;
	Pokemon* unPokemon = create_pokemon(nombrePokemon, posX, posY);
	if (send_pokemon(unPokemon, socketDescartable, false) < 0) {
		flagBrokerCaido = 1;
	}
	uint32_t idAsignado;

	if (recv(socketDescartable, &idAsignado, sizeof(uint32_t), 0) <= 0) {
		flagBrokerCaido = 1;
	}

	if (flagBrokerCaido) {
		manejadorDeEventos.registrarMensajeEnEsperaEnLista(&manejadorDeEventos, nombrePokemon, idAsignado, (&manejadorDeEventos)->listaCatchEnEspera);
	} else {
		//comportamiento caso broker caido
	}
}
