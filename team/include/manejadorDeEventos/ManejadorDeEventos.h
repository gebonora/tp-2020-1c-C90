//
// Created by Alan Zhao on 02/06/2020.
//

#ifndef TEAM_MANEJADORDEEVENTOS_H
#define TEAM_MANEJADORDEEVENTOS_H

#include "app/Global.h"
#include <pthread.h>
#include "modelo/pokemon/CapturaPokemon.h"
#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"
#include "servidor/ServidorTeam.h"

/**
 * El server va a llamar al manejador de eventos cuando recibamos los mensajes, y el cliente cuando enviemos mensajes.
 * Esta clase se va a encargar de hacer el preprocesamiento necesario para descartar mensajes por ID, y el caso especial del get, que se debe ignorar si llega
 * un localized o appeared del mismo pokemon antes que llegue la respuesta al get en cuestión. Usamos una lista para esto, sin sincronizar porque sólo se agregan elementos.
 * Va a ser el primero en enterarse si ocurre un evento que algun entrenador esperaba para desbloquearse.
 * Es el que le deja trabajo al ServicioDePlanificacion.
 *
 * Introdujimos el ServicioDeCaptura para delegar el cambio de estado de entrenadores y mapa.
 * Cuando llega un localized o appeared validado, se llama al ServicioDeCaptura.procesarPokemonCapturable().
 * Cuando llega un catch y es exitoso, se llama al ServicioDeCaptura.registrarCapturaExitosa();
 * TODO: Cuando llega un catch y no es exitoso, hay que definir que hacer. WIP en registrarCapturaFallida().
 * Los parametros que le pasemos al ServicioDeCaptura pasan a ser responsibilidad de otro para liberarlos de memoria:
 *
 *
 * servicioDeCaptura.procesarPokemonCapturable(ServicioDeCaptura * this, char * especie, Coordinate posicion)
 *                                                                       --------------
 *                                                                           esto
 *
 * servicioDeCaptura.registrarCapturaExitosa(ServicioDeCaptura * this, CapturaPokemon * capturaPokemon)
 *                                                                     -------------------------------
 *                                                                           esto
 *
 *
 */

typedef struct MensajeGet {
	char* nombrePokemon;
	uint32_t idCorrelatividad;
} MensajeGet;

typedef struct ListaSincronizada {
	t_list* lista;
	pthread_mutex_t mtx;
} ListaSincronizada;

typedef struct ManejadorDeEventos {
	t_log * logger;
	ListaSincronizada* listaGetEnEspera;
	ListaSincronizada* listaCatchEnEspera;
	t_list* listaLocalizedAppearedsRecibidos;

	// Interfaz publica
	void (*registrarCatchEnEspera)(struct ManejadorDeEventos* this, CapturaPokemon* capturaPokemon);
	void (*registrarGetEnEspera)(struct ManejadorDeEventos* this, MensajeGet* mensajeGet);

	void (*procesarLocalizedRecibido)(struct ManejadorDeEventos* this, Localized* unLocalized, uint32_t idMensaje);
	void (*procesarAppearedRecibido)(struct ManejadorDeEventos* this, Pokemon* unPokemon, uint32_t idMensaje);
	void (*procesarCaughtRecibido)(struct ManejadorDeEventos* this, Caught* unCaught, uint32_t idMensaje);

	void (*destruir)(struct ManejadorDeEventos *this);
} ManejadorDeEventos;

extern const struct ManejadorDeEventosClass {
	ManejadorDeEventos (*new)();
} ManejadorDeEventosConstructor;

ListaSincronizada* iniciarListaSincronizada();
void destruirListaSincronizada(ListaSincronizada* listaSincronizada, void (*destructorElemento)(void*));
void destruirMensajeGet(void* puntero);
void destruirCapturaPokemon(CapturaPokemon * capturaPokemon);

ManejadorDeEventos manejadorDeEventos;

#endif //TEAM_MANEJADORDEEVENTOS_H
