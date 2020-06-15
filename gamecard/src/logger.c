/*
 * logger.c
 *
 *  Created on: 20 abr. 2020
 *      Author: GONZALO BONORA
 */

#include "logger.h"

void iniciarLoggers() {
	loggerMain = log_create(PATH_LOGGER_MAIN, "Main", 1, LOG_LEVEL_INFO);
	log_info(loggerMain, "Logger Main iniciado");

	loggerNew = log_create(PATH_LOGGER_NEW, "New", 1, LOG_LEVEL_INFO);
	pthread_mutex_init(&m_loggerNew, NULL);
	log_info(loggerNew, "Logger New iniciado");

	loggerCatch = log_create(PATH_LOGGER_CATCH, "Catch", 1, LOG_LEVEL_INFO);
	pthread_mutex_init(&m_loggerCatch, NULL);
	log_info(loggerCatch, "Logger Catch iniciado");

	loggerGet = log_create(PATH_LOGGER_GET, "Get", 1, LOG_LEVEL_INFO);
	pthread_mutex_init(&m_loggerGet, NULL);
	log_info(loggerGet, "Logger Get iniciado");

	loggerGameboy = log_create(PATH_LOGGER_GAMEBOY, "Gameboy", 1, LOG_LEVEL_INFO);
	log_info(loggerGameboy, "Logger Gameboy iniciado");
}

void logearNewRecibido(New* unNew, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerNew);
	log_info(loggerNew, "Llegó un New. idMensaje: '%d', pokemon: '%s', posX: '%d', posY: '%d', cantidad: '%d'", idMensaje,
			unNew->pokemon->name->value, ((Coordinate*) (unNew->pokemon->coordinates->head->data))->pos_x,
			((Coordinate*) (unNew->pokemon->coordinates->head->data))->pos_y, unNew->quantity);
	pthread_mutex_unlock(&m_loggerNew);
}

void logearCatchRecibido(Pokemon* unPokemon, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerCatch);
	log_info(loggerCatch, "Llegó un Catch. idMensaje: '%d', pokemon: '%s', posX: '%d', posY: '%d'", idMensaje, unPokemon->name->value,
			((Coordinate*) (unPokemon->coordinates->head->data))->pos_x, ((Coordinate*) (unPokemon->coordinates->head->data))->pos_y);
	pthread_mutex_unlock(&m_loggerCatch);
}

void logearGetRecibido(Get* unGet, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerGet);
	log_info(loggerGet, "Llegó un Get. idMensaje: '%d', pokemon: '%s", idMensaje, unGet->name->value);
	pthread_mutex_unlock(&m_loggerGet);
}

