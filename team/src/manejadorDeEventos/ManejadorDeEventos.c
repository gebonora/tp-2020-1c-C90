//
// Created by Alan Zhao on 10/06/2020.
//

#include "manejadorDeEventos/ManejadorDeEventos.h"

static void procesarLocalizedRecibido(ManejadorDeEventos* this, Localized* unLocalized, uint32_t idMensaje) {
	// Llega desde Server.
	t_list* ptrLista = this->registradorDeEventos->listaGetEnEspera->lista;
	t_list* ptrListaRecibidos = this->listaLocalizedAppearedsRecibidos;
	MensajeGet* mensajeGet = NULL;

	// Consultar la lista de gets, región crítica, minimizar procesamiento.
	pthread_mutex_lock(&(this->registradorDeEventos->listaGetEnEspera->mtx));
	for (int a = 0; a < list_size(ptrLista); a++) {
		MensajeGet* auxPtr = list_get(ptrLista, a);
		if (auxPtr->idCorrelatividad == idMensaje) {
			mensajeGet = auxPtr;
			list_remove(ptrLista, a);
		}
	}
	pthread_mutex_unlock(&(this->registradorDeEventos->listaGetEnEspera->mtx));

	// Si no coincide con ningun mensaje esperado, lo ignoramos, liberar memoria y cerrar.
	if (mensajeGet == NULL) {
		char * coords = logCoordenadas(unLocalized->pokemon->coordinates);
		log_info(MANDATORY_LOGGER, "Llegó un LOCALIZED con idCorrelativo: %d, pokemon: %s%s. No coincide con ningún pedido previo. Se procede a destruirlo.", idMensaje,
				unLocalized->pokemon->name->value, coords);
		free_localized(unLocalized);
		free(coords);
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
		this->servicioDeCaptura->procesarPokemonCapturable(this->servicioDeCaptura, string_duplicate(unLocalized->pokemon->name->value), convertirACoordenada(auxCoor));
	}

	free_localized(unLocalized);
	destruirMensajeGet(mensajeGet);
}

bool pokemonEstaEnLista(ManejadorDeEventos* this, Pokemon* unPokemon) {
	for (int a = 0; a < list_size(this->listaPokemonsNecesarios); a++) {
		if (string_equals_ignore_case(unPokemon->name->value, (char*) list_get(this->listaPokemonsNecesarios, a))) {
			return true;
		}
	}
	return false;
}

static void procesarAppearedRecibido(ManejadorDeEventos* this, Pokemon* unPokemon, uint32_t idMensaje) {
	t_list* ptrLista = this->listaLocalizedAppearedsRecibidos;
	if (pokemonEstaEnLista(this, unPokemon) == false) {

		char* auxi;
		if (idMensaje == UINT32_MAX) {
			auxi = string_from_format("gameboy");
		} else {
			auxi = string_from_format("%d", idMensaje);
		}
		char* coord = logCoordenadas(unPokemon->coordinates);

		log_info(MANDATORY_LOGGER, "Llegó un APPEARED idMensaje: %s, pokemon: %s%s. Al no ser necesario para el team se procede a destruirlo...", auxi,
				unPokemon->name->value, coord);

		free(auxi);
		free(coord);
		free_pokemon(unPokemon);
		return;
	}
	// Guardamos el nombre del pokemon en la lista de recibidos, para saber de que pokemon tenemos info e ignorar sus localizeds.
	list_add(ptrLista, string_duplicate(unPokemon->name->value));
	//filtrar si el nombre del pokemon esta en la lista creada
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
	this->servicioDeCaptura->procesarPokemonCapturable(this->servicioDeCaptura, string_duplicate(unPokemon->name->value), convertirACoordenada(auxCoor));
	free_pokemon(unPokemon);
}

static void procesarCaughtRecibido(ManejadorDeEventos* this, Caught* unCaught, uint32_t idMensaje) {
	t_list* ptrLista = this->registradorDeEventos->listaCatchEnEspera->lista;
	CapturaPokemon* capturaPokemon = NULL;

	// Consultar la lista, región crítica, minimizar procesamiento.
	pthread_mutex_lock(&(this->registradorDeEventos->listaCatchEnEspera->mtx));
	for (int a = 0; a < list_size(ptrLista); a++) {
		CapturaPokemon* auxPtr = list_get(ptrLista, a);
		if (auxPtr->idCorrelatividad == idMensaje) {
			capturaPokemon = auxPtr;
			list_remove(ptrLista, a);
		}
	}
	pthread_mutex_unlock(&(this->registradorDeEventos->listaCatchEnEspera->mtx));

	// Si no coincide con ningun mensaje esperado, lo ignoramos, liberar memoria y cerrar.
	if (capturaPokemon == NULL) {
		log_info(MANDATORY_LOGGER, "Llegó un CAUGHT con idCorrelatividad: %d, resultado: %s. No coincide con ningún pedido previo. Se procede a destruirlo.", idMensaje,
				traducirResult(unCaught->result));
		free(unCaught);
		return;
	}

	log_info(MANDATORY_LOGGER, "Llegó un CAUGHT con idCorrelatividad: %d, resultado: %s. Coincide con un pedido previo. Se procede a procesarlo.", idMensaje,
			traducirResult(unCaught->result));

	// Coincide con un pedido, ver el resultado
	if (unCaught->result == FAIL) {
		// Informar que falló, liberar memoria y cerrar.
		this->servicioDeCaptura->registrarCapturaFallida(this->servicioDeCaptura, capturaPokemon);
		this->objetivoGlobalTeam.restarUnCapturado(&this->objetivoGlobalTeam, capturaPokemon->especie(capturaPokemon));
		free(unCaught);
		return;
	}
	// Caso feliz:
	this->servicioDeCaptura->registrarCapturaExitosa(this->servicioDeCaptura, capturaPokemon);
	free(unCaught);
}

static void destruir(ManejadorDeEventos * this) {
	log_destroy(this->logger);
	list_destroy_and_destroy_elements(this->listaLocalizedAppearedsRecibidos, free);
	if (this->listaPokemonsNecesarios != NULL) {
		list_destroy(this->listaPokemonsNecesarios);
	}
	free(this);
}

static void registrarpokemonsNecesarios(ManejadorDeEventos * this) {
	this->listaPokemonsNecesarios = this->objetivoGlobalTeam.especiesNecesarias(&this->objetivoGlobalTeam);
}

static void setObjetivoGlobal(ManejadorDeEventos* this, ObjetivoGlobal unObjetivo) {
	this->objetivoGlobalTeam = unObjetivo;
}

static ManejadorDeEventos* new(ServicioDeCaptura* servicioDeCaptura, RegistradorDeEventos * registradorDeEventos) {
	ManejadorDeEventos* manejador = malloc(sizeof(ManejadorDeEventos));

	manejador->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ManejadorDeEventos", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
	manejador->registradorDeEventos = registradorDeEventos;
	manejador->listaLocalizedAppearedsRecibidos = list_create();
	manejador->servicioDeCaptura = servicioDeCaptura;
	manejador->listaPokemonsNecesarios = NULL;
	manejador->registrarpokemonsNecesarios = &registrarpokemonsNecesarios;
	manejador->procesarLocalizedRecibido = &procesarLocalizedRecibido;
	manejador->procesarAppearedRecibido = &procesarAppearedRecibido;
	manejador->procesarCaughtRecibido = &procesarCaughtRecibido;
	manejador->destruir = &destruir;
	manejador->setObjetivoGlobal = &setObjetivoGlobal;

	return manejador;
}

const struct ManejadorDeEventosClass ManejadorDeEventosConstructor = { .new = &new };

// Funciones estáticas

char* traducirResult(Result result) {
	switch (result) {
	case OK:
		return "OK";
	case FAIL:
		return "FAIL";
	case ACKNOWLEDGE:
		return "ACKONWLEDGE";
	default:
		return "RESULTADO DESCONOCIDO. ALGO ANDA MAL!";
	}
}

char* logCoordenadas(t_list* listaCoor) {
	char* ret = string_new();
	for (int a = 0; a < listaCoor->elements_count; a++) {
		if (a == 0)
			string_append_with_format(&ret, ", coordenadas: (%d,%d)", ((Coordinate*) (list_get(listaCoor, a)))->pos_x, ((Coordinate*) (list_get(listaCoor, a)))->pos_y);
		else
			string_append_with_format(&ret, "|(%d,%d)", ((Coordinate*) (list_get(listaCoor, a)))->pos_x, ((Coordinate*) (list_get(listaCoor, a)))->pos_y);
	}
	return ret;
}
