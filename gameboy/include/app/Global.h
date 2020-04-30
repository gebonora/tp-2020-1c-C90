//
// Created by Alan Zhao on 22/04/2020.
//

#ifndef GAMEBOY_GLOBAL_H
#define GAMEBOY_GLOBAL_H

#include <commons/log.h>
#include <commons/config.h>
#include "delibird/protocol.h"

// Paths a archivos
#define GAMEBOY_CONFIG_FILE "config/gameboy.config"
#define GAMEBOY_INTERNAL_LOG_FILE "logs/internal_use_gameboy.log"

// Atributos del archivo de configuracion
#define IP_BROKER "IP_BROKER"
#define IP_TEAM "IP_TEAM"
#define IP_GAMECARD "IP_GAMECARD"
#define PUERTO_BROKER "PUERTO_BROKER"
#define PUERTO_TEAM "PUERTO_TEAM"
#define PUERTO_GAMECARD "PUERTO_GAMECARD"
#define MANDATORY_LOG "MANDATORY_LOG"

// Constantes
#define SHOW_INTERNAL_CONSOLE 1 // Setear en 1 temporalmente para mostrar la consola de uso interno. ¡NO PUSHEAR EL 1!

// Variables globales
t_log * MANDATORY_LOGGER;
t_log * INTERNAL_LOGGER;

#endif //GAMEBOY_GLOBAL_H
