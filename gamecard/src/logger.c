/*
 * logger.c
 *
 *  Created on: 20 abr. 2020
 *      Author: GONZALO BONORA
 */

#include "logger.h"


void iniciarLogger(){
	logger = log_create("gamecard/logs/logger.log","Server",1,LOG_LEVEL_INFO);
	log_info(logger,"Logger iniciado");

}
