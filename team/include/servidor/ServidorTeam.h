//
// Created by Alan Zhao on 07/05/2020.
//

#ifndef TEAM_SERVIDORTEAM_H
#define TEAM_SERVIDORTEAM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "app/Global.h"
#include "app/Servicios.h"



#include <commons/log.h>
#include <commons/config.h>

typedef struct {
	void* mensaje;
	uint32_t idMensaje;
} ArgumentosHilo;

void atenderConexiones();

void atenderAppeared();
void esperarBrokerAppeared(int socketDeEscucha);
void procesarHiloAppeared(Pokemon* unPokemon);

void atenderCaught();
void esperarBrokerCaught(int socketDeEscucha);
void procesarHiloCaught(ArgumentosHilo* argumentosHilo);

void atenderLocalized();
void esperarBrokerLocalized(int socketDeEscucha);
void procesarHiloLocalized(ArgumentosHilo* argumentosHilo);


int iniciarSocketDeEscucha(Operation cola, t_log* logger, pthread_mutex_t* mutex);
int subscribirseACola(Operation cola, t_log* logger, pthread_mutex_t* mutex);
char* traducirOperacion(Operation operacion);
char* traducirResult(Result result);


void atenderGameboy();

#endif //TEAM_SERVIDORTEAM_H
