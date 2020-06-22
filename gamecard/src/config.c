/*
 * config.c
 *
 *  Created on: 20 abr. 2020
 *      Author: GONZALO BONORA
 */

#include "config.h"

void cargarConfig() {
	config = config_create(PATH_CONFIG);

	log_info(loggerMain, "Archivo de configuración encontrado. Leyendo...");

	PUERTO_BROKER = string_new();
	string_append(&PUERTO_BROKER, config_get_string_value(config, "PUERTO_BROKER"));

	IP_BROKER = string_new();
	string_append(&IP_BROKER, config_get_string_value(config, "IP_BROKER"));

	PUNTO_MONTAJE_TALLGRASS = string_new();
	string_append(&PUNTO_MONTAJE_TALLGRASS, config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS"));

	TIEMPO_DE_REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	TIEMPO_DE_REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
	TIEMPO_RETARDO_OPERACION = config_get_int_value(config, "TIEMPO_RETARDO_OPERACION");

	PUERTO_GAMECARD_GAMEBOY = string_new();
	string_append(&PUERTO_GAMECARD_GAMEBOY, config_get_string_value(config, "PUERTO_GAMECARD_GAMEBOY"));

	IP_GAMECARD_GAMEBOY = string_new();
	string_append(&IP_GAMECARD_GAMEBOY, config_get_string_value(config, "IP_GAMECARD_GAMEBOY"));

	ID_GAMECARD = config_get_int_value(config, "ID_GAMECARD");

	config_destroy(config);

	log_info(loggerMain, "Config: PUERTO_BROKER: '%s'.", PUERTO_BROKER);
	log_info(loggerMain, "Config: IP_BROKER: '%s'.", IP_BROKER);
	log_info(loggerMain, "Config: PUNTO_MONTAJE_TALLGRASS: '%s'.", PUNTO_MONTAJE_TALLGRASS);
	log_info(loggerMain, "Config: TIEMPO_DE_REINTENTO_CONEXION: '%d'.", TIEMPO_DE_REINTENTO_CONEXION);
	log_info(loggerMain, "Config: TIEMPO_DE_REINTENTO_OPERACION: '%d'.", TIEMPO_DE_REINTENTO_OPERACION);
	log_info(loggerMain, "Config: TIEMPO_RETARDO_OPERACION: '%d'.", TIEMPO_RETARDO_OPERACION);
	log_info(loggerMain, "Config: PUERTO_GAMECARD_GAMEBOY: '%s'.", PUERTO_GAMECARD_GAMEBOY);
	log_info(loggerMain, "Config: IP_GAMECARD_GAMEBOY: '%s'.", IP_GAMECARD_GAMEBOY);
	log_info(loggerMain, "Config: ID_GAMECARD: '%d'.", ID_GAMECARD);
	log_info(loggerMain, "Listo!");
	puts("\n");
}

void cerrarVariablesConfig() {
	free(PUERTO_BROKER);
	free(IP_BROKER);
	free(PUNTO_MONTAJE_TALLGRASS);
	free(PUERTO_GAMECARD_GAMEBOY);
	free(IP_GAMECARD_GAMEBOY);
}

