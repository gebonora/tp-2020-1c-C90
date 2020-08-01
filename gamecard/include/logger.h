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
#include <pthread.h>

#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>

#include <delibird/interface.h>

#include "server.h"

#define LOG_SHOW_CONSOLE 0
#define AUX_LOG_LEVEL LOG_LEVEL_INFO // Pasar a info para entrega.

t_log* loggerMain;
t_log* loggerNew;
t_log* loggerCatch;
t_log* loggerGet;
t_log* loggerGameboy;
t_log* loggerAux;

pthread_mutex_t m_loggerNew;
pthread_mutex_t m_loggerCatch;
pthread_mutex_t m_loggerGet;

void iniciarLoggers();
void cerrarLoggers();

void logearNewRecibido(New* unNew, uint32_t idMensaje);
void logearCatchRecibido(Pokemon* unPokemon, uint32_t idMensaje);
void logearGetRecibido(Get* unGet, uint32_t idMensaje);

void logearAppearedProcesado(Pokemon* unAppeared, uint32_t idMensaje);
void logearCaughtProcesado(Caught* unCaught, uint32_t idMensaje);
void logearLocalizedProcesado(Localized* unLocalized, uint32_t idMensaje);

char* logCoordenadas(t_list* listaCoor);

#define PATH_LOGGER_GAMECARD "logs/LOG_GAMECARD.log"
#define PATH_LOGGER_AUX "logs/logAuxiliar.log"

#endif /* GAMECARD_INCLUDE_LOGGER_H_ */
