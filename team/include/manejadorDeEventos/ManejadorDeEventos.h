//
// Created by Alan Zhao on 02/06/2020.
//

#ifndef TEAM_MANEJADORDEEVENTOS_H
#define TEAM_MANEJADORDEEVENTOS_H

#include <pthread.h>
#include "app/Global.h"
#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"
#include "manejadorDeEventos/registro/RegistradorDeEventos.h"
#include "modelo/objetivo/ObjetivoGlobal.h"
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

typedef struct ManejadorDeEventos {
	t_log * logger;
	RegistradorDeEventos * registradorDeEventos;
	//ListaSincronizada* listaGetEnEspera;
	//ListaSincronizada* listaCatchEnEspera;
	t_list* listaLocalizedAppearedsRecibidos;
	t_list* listaPokemonsNecesarios;
	ServicioDeCaptura* servicioDeCaptura;
	ObjetivoGlobal objetivoGlobalTeam;
	// Interfaz publica
	//void (*registrarCatchEnEspera)(struct ManejadorDeEventos* this, CapturaPokemon* capturaPokemon);
	//void (*registrarGetEnEspera)(struct ManejadorDeEventos* this, MensajeGet* mensajeGet);

	void (*procesarLocalizedRecibido)(struct ManejadorDeEventos* this, Localized* unLocalized, uint32_t idMensaje);
	void (*procesarAppearedRecibido)(struct ManejadorDeEventos* this, Pokemon* unPokemon, uint32_t idMensaje);
	void (*procesarCaughtRecibido)(struct ManejadorDeEventos* this, Caught* unCaught, uint32_t idMensaje);
	void (*registrarpokemonsNecesarios)(struct ManejadorDeEventos* this);
	void (*setObjetivoGlobal)(struct ManejadorDeEventos* this, ObjetivoGlobal  unObjetivo);
	void (*destruir)(struct ManejadorDeEventos *this);
} ManejadorDeEventos;

extern const struct ManejadorDeEventosClass {
	ManejadorDeEventos* (*new)(ServicioDeCaptura* servicioDeCaptura, RegistradorDeEventos * registradorDeEventos);
} ManejadorDeEventosConstructor;

ManejadorDeEventos * manejadorDeEventosProcesoTeam;

char* logCoordenadas(t_list* listaCoor);
char* traducirResult(Result result);
bool pokemonEstaEnLista(ManejadorDeEventos* this, Pokemon* unPokemon);

#endif //TEAM_MANEJADORDEEVENTOS_H
