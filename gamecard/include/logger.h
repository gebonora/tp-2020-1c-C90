/*
 * logger.h
 *
 *  Created on: 20 abr. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_LOGGER_H_
#define GAMECARD_INCLUDE_LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <delibird/interface.h>
#include <pthread.h>

t_log* loggerMain;
t_log* loggerNew;
t_log* loggerCatch;
t_log* loggerGet;
t_log* loggerGameboy;

pthread_mutex_t m_loggerNew;
pthread_mutex_t m_loggerCatch;
pthread_mutex_t m_loggerGet;

void iniciarLoggers();
void cerrarLoggers();

void logearNewRecibido(New* unNew, uint32_t idMensaje);
void logearCatchRecibido(Pokemon* unPokemon, uint32_t idMensaje);
void logearGetRecibido(Get* unGet, uint32_t idMensaje);

#define PATH_LOGGER_MAIN "logs/loggerMain.log"
#define PATH_LOGGER_NEW "logs/loggerNew.log"
#define PATH_LOGGER_CATCH "logs/loggerCatch.log"
#define PATH_LOGGER_GET "logs/loggerGet.log"
#define PATH_LOGGER_GAMEBOY "logs/loggerGameboy.log"

#endif /* GAMECARD_INCLUDE_LOGGER_H_ */
