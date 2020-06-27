//
// Created by Alan Zhao on 07/05/2020.
//

#ifndef TEAM_GLOBAL_H
#define TEAM_GLOBAL_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "delibird/protocol.h"
#include "delibird/interface.h"

// Configuración de ambiente -> Acá definimos las de la entrega -> En el CMake se sobreescriben para desarrollo.
#ifndef TEAM_DEVELOPMENT_MODE
#define INTERNAL_LOG_LEVEL LOG_LEVEL_INFO
#define SHOW_INTERNAL_CONSOLE 0 // 1 para mostrar la consola de uso interno, 0 para el caso contrario.
#define CORRER_TESTS 0
#endif //TEAM_DEVELOPMENT_MODE

// Paths a archivos
#define TEAM_CONFIG_FILE "config/team.config"
#define TEAM_INTERNAL_LOG_FILE "logs/internal_use_team.log"

// Atributos del archivo de configuracion
#define POSICIONES_ENTRENADORES "POSICIONES_ENTRENADORES"
#define POKEMON_ENTRENADORES "POKEMON_ENTRENADORES"
#define OBJETIVOS_ENTRENADORES "OBJETIVOS_ENTRENADORES"
#define OBJETIVOS_ENTRENADORES "OBJETIVOS_ENTRENADORES"
#define TIEMPO_RECONEXION "TIEMPO_RECONEXION"
#define RETARDO_CICLO_CPU "RETARDO_CICLO_CPU"
#define ALGORITMO_PLANIFICACION "ALGORITMO_PLANIFICACION"
#define QUANTUM "QUANTUM"
#define ESTIMACION_INICIAL "ESTIMACION_INICIAL"
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"
#define LOG_FILE "LOG_FILE"
#define IP_TEAM_GAMEBOY "IP_TEAM_GAMEBOY"
#define PUERTO_TEAM_GAMEBOY "PUERTO_TEAM_GAMEBOY"

// Mutex de los recursos compartidos
pthread_mutex_t MTX_INTERNAL_LOG;

// Variables globales
t_log * MANDATORY_LOGGER;
t_log * INTERNAL_LOGGER;

#endif //TEAM_GLOBAL_H
