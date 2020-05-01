/*
 * config.h
 *
 *  Created on: 20 abr. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_CONFIG_H_
#define GAMECARD_INCLUDE_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>

t_config* config;

int TIEMPO_DE_REINTENTO_CONEXION;
int TIEMPO_DE_REINTENTO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;
char* IP_BROKER;
char* PUERTO_BROKER;

#define PATH_CONFIG "config/gameCard.config"

void cargarConfigConexiones();


#endif /* GAMECARD_INCLUDE_CONFIG_H_ */
