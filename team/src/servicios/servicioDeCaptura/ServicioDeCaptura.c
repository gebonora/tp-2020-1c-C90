//
// Created by Alan Zhao on 15/07/2020.
//

#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"

#define igualdadPokemonAtrapable(especie_, posicion) \
bool mismaEspecieMismaPosicion(void * pokemon_) { \
    PokemonAtrapable * pokemon = (PokemonAtrapable *) pokemon_; \
    return string_equals_ignore_case(pokemon->especie, especie_) && \
           pokemon->posicionInicial.pos_x == posicion.pos_x && \
           pokemon->posicionInicial.pos_y == posicion.pos_y; \
}

static PokemonAtrapable * obtenerPokemonAtrapable(ServicioDeCaptura * this, char * especie, Coordinate posicion) {
	bool mismaEspecieMismaPosicion(void * pokemon_) {
		PokemonAtrapable * pokemon = (PokemonAtrapable *) pokemon_;
		return string_equals_ignore_case(pokemon->especie, especie) && pokemon->posicionInicial.pos_x == posicion.pos_x
				&& pokemon->posicionInicial.pos_y == posicion.pos_y;
	}
	PokemonAtrapable * pokemonAtrapable = list_find(this->pokemonesAtrapables, mismaEspecieMismaPosicion);
	if (pokemonAtrapable != NULL) {
		log_debug(this->logger, "Se encontró un %s en (%d,%d) disponible para su captura", especie, posicion.pos_x, posicion.pos_y);
	} else {
		log_warning(this->logger, "No se encontró ningun %s en (%d,%d)", especie, posicion.pos_x, posicion.pos_y);
	}
	return pokemonAtrapable;
}

static void procesarPokemonCapturable(ServicioDeCaptura * this, char * especie, Coordinate posicion) {
	this->altaDePokemon(this, especie, posicion);
	bool sePuedeCapturar = false;
	if (sePuedeCapturar) {
		log_debug(this->logger, "Se determinó que es posible la captura de %s", especie);
		this->encargarTrabajoDeCaptura(this, especie, posicion);
	} else {
		log_warning(this->logger, "No hay nadie disponible que pueda capturar a %s", especie);
		free(especie);
	}
}

static PokemonAtrapable * altaDePokemon(ServicioDeCaptura * this, char * especie, Coordinate posicion) {
	char * ubicacionPokemonACapturar = separarCoordenada(posicion, "|");
	PokemonAtrapable * pokemonAtrapable = PokemonAtrapableConstructor.new(especie, ubicacionPokemonACapturar);
	registrarEnMapaPosicionPokemonAtrapable(&this->mapa, pokemonAtrapable);
	log_info(this->logger, "Agregamos un %s en la posición %s", especie, ubicacionPokemonACapturar);
	list_add(this->pokemonesAtrapables, pokemonAtrapable);
	free(ubicacionPokemonACapturar);
	return pokemonAtrapable;
}

static void encargarTrabajoDeCaptura(ServicioDeCaptura * this, char * especie, Coordinate posicion) {
	char * ubicacionPokemonACapturar = coordenadaClave(posicion);
	log_info(this->logger, "Se le encarga al servicio de planificacion que mande a un entrenador a capturar a %s en %s", especie, ubicacionPokemonACapturar);

	TrabajoPlanificador* trabajo = malloc(sizeof(TrabajoPlanificador));
	trabajo->coordenadaObjetivo = posicion;
	trabajo->objetivo = string_duplicate(especie);
	trabajo->tipo = CAPTURA_POKEMON;

	//sem_post(&this->servicioDePlanificacion->semaforoContadorColaDeTrabajo);

	free(especie);
	free(ubicacionPokemonACapturar);
}

static bool registrarCapturaExitosa(ServicioDeCaptura * this, CapturaPokemon * capturaPokemon) {
	bool sePudoRegistrar = false;
	log_debug(this->logger, "Intentando impactar en el sistema la captura de %s por parte de %s", capturaPokemon->especie(capturaPokemon),
			capturaPokemon->idEntrenador(capturaPokemon));

	Entrenador * entrenador = capturaPokemon->entrenador;

	log_debug(this->logger, "Eliminando al pokemon capturado del mapa...");
	bool sePudoEliminarDelMapa = capturaPokemon->eliminarPokemonCapturadoDelMapa(capturaPokemon);

	if (sePudoEliminarDelMapa) {
		entrenador->registrarCaptura(entrenador, capturaPokemon->especie(capturaPokemon));

		igualdadPokemonAtrapable(capturaPokemon->especie(capturaPokemon), capturaPokemon->pokemonAtrapable->posicionInicial);
		list_remove_by_condition(this->pokemonesAtrapables, mismaEspecieMismaPosicion);
		sePudoRegistrar = true;
		char * posicion = capturaPokemon->posicion(capturaPokemon);
		log_info(this->logger, "%s capturó con exito un %s en %s", capturaPokemon->idEntrenador(capturaPokemon), capturaPokemon->pokemonAtrapable->especie, posicion);

		capturaPokemon->entrenador->estaEsperandoAlgo = false;

		free(posicion);
	} else {
		log_error(this->logger, "No se puede capturar un pokemon que no figure en el mapa");
	}

	capturaPokemon->destruir(capturaPokemon);

	return sePudoRegistrar;
}

static bool registrarCapturaFallida(ServicioDeCaptura * this, CapturaPokemon * capturaPokemon) {
	bool sePudoRegistrar = false;
	log_debug(this->logger, "Intentando impactar en el sistema la captura FALLIDA de %s por parte de %s", capturaPokemon->especie(capturaPokemon),
			capturaPokemon->idEntrenador(capturaPokemon));

	Entrenador * entrenador = capturaPokemon->entrenador;

	log_debug(this->logger, "Eliminando al pokemon del mapa, no se encontró en la coordenadas esperadas...");
	bool sePudoEliminarDelMapa = capturaPokemon->eliminarPokemonCapturadoDelMapa(capturaPokemon);

	if (sePudoEliminarDelMapa) {
		// TODO: Asegurarse de que cuando se genera la tarea se ponga este flag en true. En ServicioDePlanificacion.
		capturaPokemon->entrenador->estaEsperandoAlgo = false;
		sePudoRegistrar = true;
		char * posicion = capturaPokemon->posicion(capturaPokemon);
		log_info(this->logger, "%s eliminó las coordenadas de un %s en %s porque falló su captura.", capturaPokemon->idEntrenador(capturaPokemon),
				capturaPokemon->pokemonAtrapable->especie, posicion);
		free(posicion);
	} else {
		log_error(this->logger, "No se puede eliminar un pokemon que no figure en el mapa");
	}

	capturaPokemon->destruir(capturaPokemon);

	return sePudoRegistrar;
}

void destruirServicioDeCaptura(ServicioDeCaptura * this) {
	log_debug(this->logger, "Se procede a destruir al servicio de captura");
	log_destroy(this->logger);
	list_destroy_and_destroy_elements(this->pokemonesAtrapables, (void (*)(void *)) destruirPokemonAtrapable);
	free(this);
}

static ServicioDeCaptura * new(Mapa mapa, ServicioDePlanificacion * servicioDePlanificacion) {
	ServicioDeCaptura * servicio = malloc(sizeof(ServicioDeCaptura));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeCaptura", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
	servicio->mapa = mapa;
	servicio->pokemonesAtrapables = list_create();
	servicio->servicioDePlanificacion = servicioDePlanificacion;
	servicio->registrarCapturaExitosa = &registrarCapturaExitosa;
	servicio->registrarCapturaFallida = &registrarCapturaFallida;
	servicio->procesarPokemonCapturable = &procesarPokemonCapturable;
	servicio->encargarTrabajoDeCaptura = &encargarTrabajoDeCaptura;
	servicio->altaDePokemon = &altaDePokemon;
	servicio->obtenerPokemonAtrapable = &obtenerPokemonAtrapable;
	servicio->destruir = &destruirServicioDeCaptura;

	return servicio;
}

const struct ServicioDeCapturaClass ServicioDeCapturaConstructor = { .new = &new };

void destruirPokemonAtrapable(PokemonAtrapable * pokemonAtrapable) {
	pokemonAtrapable->destruir(pokemonAtrapable);
}
