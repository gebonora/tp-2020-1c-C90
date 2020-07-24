/*
 * logger.c
 *
 *  Created on: 20 abr. 2020
 *      Author: GONZALO BONORA
 */
#include "logger.h"

void iniciarLoggers() {
	loggerMain = log_create(PATH_LOGGER_GAMECARD, "Main", LOG_SHOW_CONSOLE, LOG_LEVEL_DEBUG);
	log_debug(loggerMain, "Logger Main iniciado.");

	loggerNew = log_create(PATH_LOGGER_GAMECARD, "New", LOG_SHOW_CONSOLE, LOG_LEVEL_DEBUG);
	pthread_mutex_init(&m_loggerNew, NULL);
	log_debug(loggerNew, "Logger New iniciado.");

	loggerCatch = log_create(PATH_LOGGER_GAMECARD, "Catch", LOG_SHOW_CONSOLE, LOG_LEVEL_DEBUG);
	pthread_mutex_init(&m_loggerCatch, NULL);
	log_debug(loggerCatch, "Logger Catch iniciado.");

	loggerGet = log_create(PATH_LOGGER_GAMECARD, "Get", LOG_SHOW_CONSOLE, LOG_LEVEL_DEBUG);
	pthread_mutex_init(&m_loggerGet, NULL);
	log_debug(loggerGet, "Logger Get iniciado.");

	loggerGameboy = log_create(PATH_LOGGER_GAMECARD, "Gameboy", LOG_SHOW_CONSOLE, LOG_LEVEL_DEBUG);
	log_debug(loggerGameboy, "Logger Gameboy iniciado.");
	puts("\n");
}

void logearNewRecibido(New* unNew, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerNew);
	log_info(loggerNew, "Llegó un New. idMensaje: '%d', pokemon: '%s', posX: '%d', posY: '%d', cantidad: '%d'.", idMensaje, unNew->pokemon->name->value,
			((Coordinate*) (unNew->pokemon->coordinates->head->data))->pos_x, ((Coordinate*) (unNew->pokemon->coordinates->head->data))->pos_y, unNew->quantity);
	pthread_mutex_unlock(&m_loggerNew);
}

void logearCatchRecibido(Pokemon* unPokemon, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerCatch);
	log_info(loggerCatch, "Llegó un Catch. idMensaje: '%d', pokemon: '%s', posX: '%d', posY: '%d'.", idMensaje, unPokemon->name->value,
			((Coordinate*) (unPokemon->coordinates->head->data))->pos_x, ((Coordinate*) (unPokemon->coordinates->head->data))->pos_y);
	pthread_mutex_unlock(&m_loggerCatch);
}

void logearGetRecibido(Get* unGet, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerGet);
	log_info(loggerGet, "Llegó un Get. idMensaje: '%d', pokemon: '%s'.", idMensaje, unGet->name->value);
	pthread_mutex_unlock(&m_loggerGet);
}

void logearAppearedProcesado(Pokemon* unAppeared, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerNew);
	log_info(loggerNew, "Se generó un Appeared como respuesta al idMensaje: '%d', pokemon: '%s', posX: '%d', posY: '%d'.", idMensaje, unAppeared->name->value,
			((Coordinate*) (unAppeared->coordinates->head->data))->pos_x, ((Coordinate*) (unAppeared->coordinates->head->data))->pos_y);
	pthread_mutex_unlock(&m_loggerNew);
}

void logearCaughtProcesado(Caught* unCaught, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerCatch);
	log_info(loggerCatch, "Se generó un Caught como respuesta al idMensaje: '%d', resultado: '%s'.", idMensaje, traducirResult(unCaught->result));
	pthread_mutex_unlock(&m_loggerCatch);
}

void logearLocalizedProcesado(Localized* unLocalized, uint32_t idMensaje) {
	pthread_mutex_lock(&m_loggerGet);
	char* stringCoor = logCoordenadas(unLocalized->pokemon->coordinates);
	log_info(loggerGet, "Se generó un Localized como respuesta al idMensaje: '%d', pokemon: '%s', cantidadCoordenadas: '%d'%s.", idMensaje,
			unLocalized->pokemon->name->value, unLocalized->coordinates_quantity, stringCoor);
	free(stringCoor);
	pthread_mutex_unlock(&m_loggerGet);
}

char* logCoordenadas(t_list* listaCoor) {
	char* ret = string_new();
	for (int a = 0; a < listaCoor->elements_count; a++) {
		if (a == 0)
			string_append_with_format(&ret, " ,coordenadas: '(%d,%d)'", ((Coordinate*) (list_get(listaCoor, a)))->pos_x, ((Coordinate*) (list_get(listaCoor, a)))->pos_y);
		else
			string_append_with_format(&ret, "|'(%d,%d)'", ((Coordinate*) (list_get(listaCoor, a)))->pos_x, ((Coordinate*) (list_get(listaCoor, a)))->pos_y);
	}
	return ret;
}

void cerrarLoggers() {
	log_destroy(loggerMain);
	log_destroy(loggerNew);
	log_destroy(loggerCatch);
	log_destroy(loggerGet);
	log_destroy(loggerGameboy);
	pthread_mutex_destroy(&m_loggerNew);
	pthread_mutex_destroy(&m_loggerCatch);
	pthread_mutex_destroy(&m_loggerGet);
}

