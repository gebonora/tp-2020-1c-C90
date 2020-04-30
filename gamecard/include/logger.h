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

t_log* logger;

void iniciarLogger();

#define PATH_LOGGER "/home/utnso/tp-2020-1c-C90/gamecard/logs/logger.log"


#endif /* GAMECARD_INCLUDE_LOGGER_H_ */
