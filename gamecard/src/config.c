/*
 * config.c
 *
 *  Created on: 20 abr. 2020
 *      Author: GONZALO BONORA
 */

#include "config.h"

void cargarConfigConexiones() {
	config = config_create(PATH_CONFIG);
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUNTO_MONTAJE_TALLGRASS = config_get_string_value(config,
			"PUNTO_MONTAJE_TALLGRASS");
	TIEMPO_DE_REINTENTO_CONEXION = config_get_int_value(config,
			"TIEMPO_DE_REINTENTO_CONEXION");
	TIEMPO_DE_REINTENTO_OPERACION = config_get_int_value(config,
			"TIEMPO_DE_REINTENTO_OPERACION");
	PUERTO_GAMECARD_GAMEBOY = config_get_string_value(config,
			"PUERTO_GAMECARD_GAMEBOY");
	IP_GAMECARD_GAMEBOY = config_get_string_value(config,
			"IP_GAMECARD_GAMEBOY");
}

