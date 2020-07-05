//
// Created by Alan Zhao on 02/06/2020.
//

#ifndef TEAM_MANEJADORDEEVENTOS_H
#define TEAM_MANEJADORDEEVENTOS_H

#include "app/Global.h"

/**
 * El server va a llamar al manejador de eventos cuando recibamos los mensajes.
 * Esta clase se va a encargar de hacer el preprocesamiento necesario para descartar mensajes por ID.
 * Va a ser el primero en enterarse si ocurre un evento que algun entrenador esperaba para desbloquearse.
 * Es el que le deja trabajo al ServicioDePlanificacion.
 *
 * Problema:
 *  - Debe faciltar una manera de que los entrenadores informen que esperan cierto evento para continuar su life cycle.
 *      Tal vez con un this.registrarEventoEsperado()
 * TODO: la notificacion del evento localized y appeared se puede generalizar en uno o mas "aparecioPokemon".
 */

typedef struct ManejadorDeEventos {
    t_log * logger;
    // Interfaz publica
    void (*notificarEventoLocalized)(struct ManejadorDeEventos *this);
    void (*destruir)(struct ManejadorDeEventos *this);
} ManejadorDeEventos;

extern const struct ManejadorDeEventosClass {
    ManejadorDeEventos (*new)();
} ManejadorDeEventosConstructor;

ManejadorDeEventos manejadorDeEventos;

#endif //TEAM_MANEJADORDEEVENTOS_H
