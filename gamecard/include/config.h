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

#include "logger.h"

t_config* config;

int TIEMPO_DE_REINTENTO_CONEXION;
int TIEMPO_DE_REINTENTO_OPERACION;
int TIEMPO_RETARDO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;
char* IP_BROKER;
char* PUERTO_BROKER;
char* IP_GAMECARD_GAMEBOY;
char* PUERTO_GAMECARD_GAMEBOY;

#define PATH_CONFIG "config/gameCard.config"

void cargarConfig();
void cerrarVariablesConfig();

#endif /* GAMECARD_INCLUDE_CONFIG_H_ */
