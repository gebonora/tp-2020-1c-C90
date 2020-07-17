//
// Created by Alan Zhao on 10/06/2020.
//

#include "manejadorDeEventos/ManejadorDeEventos.h"

static void registrarCatchEnEspera(ManejadorDeEventos* this, CapturaPokemon* capturaPokemon) {
	// Llega desde Cliente.
	pthread_mutex_lock(&(this->listaCatchEnEspera->mtx));
	list_add(this->listaCatchEnEspera->lista, capturaPokemon);
	pthread_mutex_unlock(&(this->listaCatchEnEspera->mtx));
	char* coor = capturaPokemon->posicion(capturaPokemon);
	log_debug(this->logger, "Se agregó una CapturaPokemon a la listaDeCatchsEnEspera para entrenadorId: %s, pokemon: %s,coordenadas: %s, idCorrelativo: %d.",
			capturaPokemon->idEntrenador, capturaPokemon->pokemonAtrapable->especie, coor, capturaPokemon->idCorrelatividad);
	free(coor);
}

static void registrarGetEnEspera(ManejadorDeEventos* this, MensajeGet* mensajeGet) {
	// Llega desde Cliente.
	pthread_mutex_lock(&(this->listaGetEnEspera->mtx));
	list_add(this->listaGetEnEspera->lista, mensajeGet);
	pthread_mutex_unlock(&(this->listaGetEnEspera->mtx));
	log_debug(this->logger, "Se agregó un MensajeGet a la listaDeGetsEnEspera para pokemon: %s, idCorrelativo: %d.", mensajeGet->nombrePokemon, mensajeGet->idCorrelatividad);
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
		log_info(MANDATORY_LOGGER, "Llegó un LOCALIZED con idCorrelativo: %d, pokemon: %s%s. No coincide con ningún pedido previo. Se procede a destruirlo.", idMensaje,
				unLocalized->pokemon->name->value, logCoordenadas(unLocalized->pokemon->coordinates));
		free_localized(unLocalized);
		return;
	}

	// Consultar lista de recibidos, si ya está lo ignoramos, liberar memoria y cerrar.
	for (int a = 0; a < list_size(ptrListaRecibidos); a++) {
		if (string_equals(list_get(ptrListaRecibidos, a), unLocalized->pokemon->name->value)) {
			// Liberar memoria y cerrar.
			char* coor = logCoordenadas(unLocalized->pokemon->coordinates);
			log_info(MANDATORY_LOGGER,
					"Llegó un LOCALIZED con idCorrelativo: %d, pokemon: %s%s. Coincide con un pedido previo. Como ya se recibió información de este pokemon previamente se procede a destruirlo.",
					idMensaje, unLocalized->pokemon->name->value, coor);
			free(coor);
			destruirMensajeGet(mensajeGet);
			free_localized(unLocalized);
			return;
		}
	}

	// Agregamos a lista de recibidos
	list_add(ptrListaRecibidos, string_duplicate(unLocalized->pokemon->name->value));

	char* coor = logCoordenadas(unLocalized->pokemon->coordinates);
	log_info(MANDATORY_LOGGER, "Llegó un LOCALIZED con idCorrelativo: %d, pokemon: %s%s. Coincide con un pedido previo. Se procede a procesarlo.", idMensaje,
			unLocalized->pokemon->name->value, coor);
	free(coor);
	// Foreach coordenada llamar al ServicioDeCaptura;
	t_list* coordenadas = unLocalized->pokemon->coordinates;
	for (int a = 0; a < list_size(coordenadas); a++) {
		Coordinate* auxCoor = list_get(coordenadas, a);
		// Quiero pasarle al servicio una copia de los datos, para evitar leaks.
		servicioDeCapturaProcesoTeam->procesarPokemonCapturable(servicioDeCapturaProcesoTeam,string_duplicate(unLocalized->pokemon->name->value), convertirACoordenada(auxCoor));
	}

	free_localized(unLocalized);
	destruirMensajeGet(mensajeGet);
}

static void procesarAppearedRecibido(ManejadorDeEventos* this, Pokemon* unPokemon, uint32_t idMensaje) {
	// Llega desde Server.
	t_list* ptrLista = this->listaLocalizedAppearedsRecibidos;
	// Guardamos el nombre del pokemon en la lista de recibidos, para saber de que pokemon tenemos info e ignorar sus localizeds.
	list_add(ptrLista, string_duplicate(unPokemon->name->value));

	char* aux;
		if (idMensaje == UINT32_MAX) {
			aux = string_from_format("gameboy");
		} else {
			aux = string_from_format("%d", idMensaje);
		}
	char* coor = logCoordenadas(unPokemon->coordinates);
	log_info(MANDATORY_LOGGER, "Llegó un APPEARED con idMensaje: %s, pokemon: %s%s. Se procede a procesarlo.", aux, unPokemon->name->value, coor);
	free(aux);
	free(coor);

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
		log_info(MANDATORY_LOGGER, "Llegó un CAUGHT con idCorrelatividad: %d, resultado: %s. No coincide con ningún pedido previo. Se procede a destruirlo.", idMensaje, traducirResult(unCaught->result));
		free(unCaught);
		return;
	}

	log_info(MANDATORY_LOGGER, "Llegó un CAUGHT con idCorrelatividad: %d, resultado: %s. Coincide con un pedido previo. Se procede a procesarlo.", idMensaje, traducirResult(unCaught->result));

	// Coincide con un pedido, ver el resultado
	if (unCaught->result == FAIL) {
		// Informar que falló, liberar memoria y cerrar.
		free(unCaught);
		return;
	}
	// Caso feliz:
	servicioDeCapturaProcesoTeam->registrarCapturaExitosa(servicioDeCapturaProcesoTeam, capturaPokemon);
	//servicioDeCapturaProcesoTeam->
	free(unCaught);
}

static void destruir(ManejadorDeEventos * this) {
	log_destroy(this->logger);
	destruirListaSincronizada(this->listaGetEnEspera, destruirMensajeGet);
	list_destroy_and_destroy_elements(this->listaLocalizedAppearedsRecibidos, free);
    destruirListaSincronizada(this->listaCatchEnEspera, (void (*)(void *)) destruirCapturaPokemon);
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

void destruirCapturaPokemon(CapturaPokemon * capturaPokemon) {
    capturaPokemon->destruir(capturaPokemon);
}

