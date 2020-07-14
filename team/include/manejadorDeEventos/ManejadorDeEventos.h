//
// Created by Alan Zhao on 02/06/2020.
//

#ifndef TEAM_MANEJADORDEEVENTOS_H
#define TEAM_MANEJADORDEEVENTOS_H

#include "app/Global.h"
#include <pthread.h>

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

typedef struct MensajeEsperado {
	char* nombrePokemon;
	uint32_t idMensaje;
} MensajeEsperado;

typedef struct ListaSincronizada{
	t_list* lista;
	pthread_mutex_t mtx;
}ListaSincronizada;

typedef struct ManejadorDeEventos {
	t_log * logger;
	ListaSincronizada* listaGetEnEspera;
	ListaSincronizada* listaCatchEnEspera;

	// Interfaz publica
	void (*registrarMensajeEnEsperaEnLista)(struct ManejadorDeEventos* this, char* nombrePokemon, uint32_t idAsignado, ListaSincronizada* lista);

	void (*notificarNuevoPokemon)(struct ManejadorDeEventos* this);
	void (*notificarPokemonCapturado)(struct ManejadorDeEventos* this);

	void (*procesarLocalizedRecibido)(struct ManejadorDeEventos* this, Localized* unLocalized);
	void (*procesarAppearedRecibido)(struct ManejadorDeEventos* this, Pokemon* unPokemon);
	void (*procesarCaughtRecibido)(struct ManejadorDeEventos* this, Caught* unCaught, uint32_t idMensaje);

	void (*destruir)(struct ManejadorDeEventos *this);
} ManejadorDeEventos;

extern const struct ManejadorDeEventosClass {
	ManejadorDeEventos (*new)();
} ManejadorDeEventosConstructor;

ListaSincronizada* iniciarListaSincronizada();

ManejadorDeEventos manejadorDeEventos;

#endif //TEAM_MANEJADORDEEVENTOS_H
