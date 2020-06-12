//
// Created by Alan Zhao on 07/05/2020.
//

#ifndef TEAM_GLOBAL_H
#define TEAM_GLOBAL_H

#include <commons/log.h>
#include <commons/config.h>
#include "delibird/protocol.h"
#include "delibird/interface.h"


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

// Constantes
#define SHOW_INTERNAL_CONSOLE 1 // Setear en 1 para mostrar la consola de uso interno. Dejar en 0 para la entrega.

//mutex de los recursos compartidos

pthread_mutex_t MTX_INTERNAL_LOG;

// Variables globales
t_log * MANDATORY_LOGGER;
t_log * INTERNAL_LOGGER;

#endif //TEAM_GLOBAL_H
