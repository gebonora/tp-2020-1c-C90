//
// Created by Alan Zhao on 19/07/2020.
//

#ifndef TEAM_CLIENTEBROQUER_H
#define TEAM_CLIENTEBROQUER_H

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
//#include "manejadorDeEventos/ManejadorDeEventos.h" -----
//#include "modelo/pokemon/CapturaPokemon.h"
//#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"
//#include "servidor/ServidorTeam.h"
//#include "servicios/servicioDeCaptura/ServicioDeCaptura.h" ---
//#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/equipo/Equipo.h"
#include "modelo/pokemon/PokemonAtrapable.h"
//#include "modelo/pokemon/CapturaPokemon.h"
#include "modelo/mapa/coordenadas/UtilidadesCoordenadas.h"
//#include "modelo/mapa/registro/RegistradorDePosiciones.h"

/*
 * Tratamos de dejar al entrenador como un ente piola separado del resto.
 * La cosa se complica cuando hay que llamar al cliente broker.
 *  Obtiene respuesta
 */

#endif //TEAM_CLIENTEBROQUER_H
