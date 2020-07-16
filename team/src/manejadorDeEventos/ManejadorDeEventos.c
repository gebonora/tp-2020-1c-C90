//
// Created by Alan Zhao on 10/06/2020.
//

#include "manejadorDeEventos/ManejadorDeEventos.h"

static void registrarCatchEnEspera(ManejadorDeEventos* this, CapturaPokemon* capturaPokemon) {
	// Llega desde Cliente.
	pthread_mutex_lock(&(this->listaCatchEnEspera->mtx));
	list_add(this->listaCatchEnEspera->lista, capturaPokemon);
	pthread_mutex_unlock(&(this->listaCatchEnEspera->mtx));
}

static void registrarGetEnEspera(ManejadorDeEventos* this, MensajeGet* mensajeGet) {
	// Llega desde Cliente.
	pthread_mutex_lock(&(this->listaGetEnEspera->mtx));
	list_add(this->listaGetEnEspera->lista, mensajeGet);
	pthread_mutex_unlock(&(this->listaGetEnEspera->mtx));
}

static void procesarLocalizedRecibido(ManejadorDeEventos* this, Localized* unLocalized, uint32_t idMensaje) {
	// Llega desde Server.
	t_list* ptrLista = this->listaGetEnEspera->lista;
	t_list* ptrListaRecibidos = this->listaLocalizedAppearedsRecibidos;
	MensajeGet* mensajeGet = NULL;

	// Consultar la lista de gets, región crítica, minimizar procesamiento.
	pthread_mutex_lock(&(this->listaGetEnEspera->mtx));
	for (int a = 0; a < list_size(ptrLista); a++) {
		MensajeGet* auxPtr = list_get(ptrLista, a);
		if (auxPtr->idCorrelatividad == idMensaje) {
			mensajeGet = auxPtr;
			list_remove(ptrLista, a);
		}
	}
	pthread_mutex_unlock(&(this->listaGetEnEspera->mtx));

	// Si no coincide con ningun mensaje esperado, lo ignoramos, liberar memoria y cerrar.
	if (mensajeGet == NULL) {
		free_localized(unLocalized);
		return;
	}

	// Consultar lista de recibidos, si ya está lo ignoramos, liberar memoria y cerrar.
	for (int a = 0; a < list_size(ptrListaRecibidos); a++) {
		if (string_equals(list_get(ptrListaRecibidos, a), unLocalized->pokemon->name->value)) {
			//liberar memoria y cerrar.
			free_localized(unLocalized);
			destruirMensajeGet(mensajeGet);
			return;
		}
	}

	// Agregamos a lista de recibidos
	list_add(ptrListaRecibidos, string_duplicate(unLocalized->pokemon->name->value));

	// Foreach coordenada llamar al ServicioDeCaptura;
	t_list* coordenadas = unLocalized->pokemon->coordinates;
	for (int a = 0; a < list_size(coordenadas); a++) {
		Coordinate* auxCoor = list_get(coordenadas, a);
		// Quiero pasare al servicio una copia de los datos, para evitar leaks.
		servicioDeCapturaProcesoTeam->procesarPokemonCapturable(servicioDeCapturaProcesoTeam,string_duplicate(unLocalized->pokemon->name->value), convertirACoordenada(auxCoor));
	}

	free_localized(unLocalized);
	destruirMensajeGet(mensajeGet);
}

static void procesarAppearedRecibido(ManejadorDeEventos* this, Pokemon* unPokemon) {
	// Llega desde Server.
	t_list* ptrLista = this->listaLocalizedAppearedsRecibidos;
	// Guardamos el nombre del pokemon en la lista de recibidos, para saber de que pokemon tenemos info e ignorar sus localizeds.
	list_add(ptrLista, string_duplicate(unPokemon->name->value));

	Coordinate* auxCoor = list_get(unPokemon->coordinates, 0);
	servicioDeCapturaProcesoTeam->procesarPokemonCapturable(servicioDeCapturaProcesoTeam, string_duplicate(unPokemon->name->value), convertirACoordenada(auxCoor));
	free_pokemon(unPokemon);
}

static void procesarCaughtRecibido(ManejadorDeEventos* this, Caught* unCaught, uint32_t idMensaje) {
	// Llega desde Server.
	t_list* ptrLista = this->listaCatchEnEspera->lista;
	CapturaPokemon* capturaPokemon = NULL;

	// Consultar la lista, región crítica, minimizar procesamiento.
	pthread_mutex_lock(&(this->listaCatchEnEspera->mtx));
	for (int a = 0; a < list_size(ptrLista); a++) {
		CapturaPokemon* auxPtr = list_get(ptrLista, a);
		if (auxPtr->idCorrelatividad == idMensaje) {
			capturaPokemon = auxPtr;
			list_remove(ptrLista, a);
		}
	}
	pthread_mutex_unlock(&(this->listaCatchEnEspera->mtx));

	// Si no coincide con ningun mensaje esperado, lo ignoramos, liberar memoria y cerrar.
	if (capturaPokemon == NULL) {
		free(unCaught);
		return;
	}

	// Coincide con un pedido, ver el resultado
	if (unCaught->result == FAIL) {
		// Informar que falló, liberar memoria y cerrar.
		free(unCaught);
		return;
	}
	// Caso feliz:
	servicioDeCapturaProcesoTeam->registrarCapturaExitosa(servicioDeCapturaProcesoTeam, capturaPokemon);
	free(unCaught);
}

static void destruir(ManejadorDeEventos * this) {
	log_destroy(this->logger);
	destruirListaSincronizada(this->listaGetEnEspera, destruirMensajeGet);
	list_destroy_and_destroy_elements(this->listaLocalizedAppearedsRecibidos, free);
	// TODO: destruir ListaCatchEnEspera, ver si se puede liberar la memoria acá o CapturaPokemon se está liberando en otro lado.
}

static ManejadorDeEventos new() {
	return (ManejadorDeEventos ) {
		.logger = log_create(TEAM_INTERNAL_LOG_FILE, "ManejadorDeEventos", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
		.listaGetEnEspera =	iniciarListaSincronizada(),
		.listaCatchEnEspera = iniciarListaSincronizada(),
		.listaLocalizedAppearedsRecibidos = list_create(),
		&registrarCatchEnEspera,
		&registrarGetEnEspera,
		&procesarLocalizedRecibido,
		&procesarAppearedRecibido,
		&procesarCaughtRecibido,
		&destruir,
	} ;
}

const struct ManejadorDeEventosClass ManejadorDeEventosConstructor = { .new = &new };

ListaSincronizada* iniciarListaSincronizada() {
	ListaSincronizada* listaSincronizada = malloc(sizeof(ListaSincronizada));
	listaSincronizada->lista = list_create();
	pthread_mutex_init(&(listaSincronizada->mtx), NULL);
	return listaSincronizada;
}

void destruirListaSincronizada(ListaSincronizada* listaSincronizada, void (*destructorElemento)(void*)) {
	pthread_mutex_destroy(&(listaSincronizada->mtx));
	list_destroy_and_destroy_elements(listaSincronizada->lista, destructorElemento);
	free(listaSincronizada);
}

void destruirMensajeGet(void* puntero) {
	MensajeGet* mensajeGet = (MensajeGet*) puntero;
	free(mensajeGet->nombrePokemon);
	free(mensajeGet);
}
