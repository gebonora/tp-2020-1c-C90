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
#include <pthread.h>
#include <signal.h>
#include "app/Global.h"
#include "utils/sockets/Sockets.h"
#include "cliente/ClienteBroker.h"

typedef struct {
	void* mensaje;
	uint32_t idMensaje;
} ArgumentosHilo;


char* IP_Broker;
char* IP_Team_Gameboy;
char* Puerto_Broker;
char* Puerto_Team_Gameboy;
int Tiempo_Reconexion;

void configurarServer();
void eliminarConfigServer();

void atenderConexiones();

void atenderAppeared();
void esperarBrokerAppeared(int socketDeEscucha);
void procesarHiloAppeared(ArgumentosHilo* argumentosHilo);

void atenderCaught();
void esperarBrokerCaught(int socketDeEscucha);
void procesarHiloCaught(ArgumentosHilo* argumentosHilo);

void atenderLocalized();
void esperarBrokerLocalized(int socketDeEscucha);
void procesarHiloLocalized(ArgumentosHilo* argumentosHilo);

int iniciarSocketDeEscucha(Operation cola);
int subscribirseACola(Operation cola, t_log* logger);
char* traducirOperacion(Operation operacion);
char* traducirResult(Result result);

char* logCoordenadas(t_list* listaCoor);

void atenderGameboy();

void apagarServer();

// Variables globales del server
int SOCKET_APPEARED;
int SOCKET_CAUGHT;
int SOCKET_LOCALIZED;
int SOCKET_GAMEBOY;

pthread_t threadAppeared, threadCaught, threadLocalized, threadGameboy;

#endif //TEAM_SERVIDORTEAM_H
