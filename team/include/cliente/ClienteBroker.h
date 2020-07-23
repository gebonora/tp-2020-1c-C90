//
// Created by Alan Zhao on 10/06/2020.
//

#ifndef TEAM_CLIENTEBROKER_H
#define TEAM_CLIENTEBROKER_H

/**
 * Idealmente, los mensajes que quiera enviarle al broker van a salir por aca.
 * Esta clase nos expone las formas de dejar un mensaje en el broker.
 * Ejemplo: Quiero atrapar a un pokemon, necesito enviar un CATCH.
 * Probablemente se pueda generalizar y mandar a libs.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include "app/Global.h"
#include "delibird/utils/sockets/Sockets.h"
#include "manejadorDeEventos/ManejadorDeEventos.h"
#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"

typedef struct ClienteBroker {
	t_log * logger;
	char* ipBroker;
	char* puertoBroker;
	ManejadorDeEventos* manejadorDeEventos;
	ServicioDeCaptura* servicioDeCaptura;
	void (*enviarGet)(struct ClienteBroker * this, char* nombrePokemon);
	void (*enviarCatch)(struct ClienteBroker * this, CapturaPokemon* capturaPokemon);
	void (*destruir)(struct ClienteBroker * this);
} ClienteBroker;

extern const struct ClienteBrokerClass {
	ClienteBroker * (*new)(ManejadorDeEventos* manejadorDeEventos, ServicioDeCaptura* servicioDeCaptura);
} ClienteBrokerConstructor;

ClienteBroker* clienteBrokerProcesoTeam;

#endif //TEAM_CLIENTEBROKER_H
