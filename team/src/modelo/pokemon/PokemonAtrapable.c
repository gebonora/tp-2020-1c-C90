//
// Created by Alan Zhao on 15/07/2020.
//

#include "modelo/pokemon/PokemonAtrapable.h"

static bool borrarPosicion(PokemonAtrapable * this) {
    Gps * gps = this->gps;
    if (gps == NULL) {
        return false;
    }
    bool sePudoBorrar = gps->borrarPosicionDelMapa(gps);
    if (sePudoBorrar) {
        this->gps->destruirGps(this->gps);
        this->gps = NULL;
    }
    return sePudoBorrar;
}

static Posicion posicion(PokemonAtrapable * this) {
    Gps * gps = this->gps;
    if (gps == NULL) {
        log_error(this->logger, "El pokemon no tiene un GPS asignado. No puede responder su ubicación.");
        return (Posicion) {.valida = false};
    }
    return gps->posicionActual(gps);
}

static char * descripcion(PokemonAtrapable * this) {
    return this->especie;
}

static bool esAtrapable(PokemonAtrapable * this) {
    bool atrapable;
    pthread_mutex_lock(&this->mutexAtrapable);
    atrapable = this->disponible;
    pthread_mutex_unlock(&this->mutexAtrapable);
    return atrapable;
}

static bool marcarComoObjetivo(PokemonAtrapable * this, char * idMarcador) {
    if (this->disponible) {
        pthread_mutex_lock(&this->mutexAtrapable);
        this->disponible = false;
        this->idMarcador = idMarcador;
        pthread_mutex_unlock(&this->mutexAtrapable);
        return true;
    } else {
        log_error(this->logger, "Este pokemon ya fue marcado por otro entrenador -> %s", this->idMarcador);
        return false;
    }
}

static void destruirPokemonAtrapable(PokemonAtrapable * this) {
    if (this->gps != NULL) {
        Posicion posicion = this->posicion(this);
        char * coordenada = coordenadaClave(posicion.coordenada);
        log_debug(this->logger, "Se procede a destruir un %s en %s", this->especie, coordenada);
        free(coordenada);
        this->gps->destruirGps(this->gps);
    } else {
        char * posicionInicial = coordenadaClave(this->posicionInicial);
        log_debug(this->logger, "Se procede a destruir un %s que debería estar en %s, pero nunca se registró.", this->especie, posicionInicial);
        free(posicionInicial);
    }
    log_destroy(this->logger);
    pthread_mutex_destroy(&this->mutexAtrapable);
    free(this);
}

static PokemonAtrapable * new(char * especie, char * posicionInicial) {
    PokemonAtrapable * pokemonAtrapable = malloc(sizeof(PokemonAtrapable));

    pokemonAtrapable->logger = log_create(TEAM_INTERNAL_LOG_FILE, "PokemonAtrapable", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    pokemonAtrapable->especie = especie;
    pokemonAtrapable->gps = NULL;
    pokemonAtrapable->tipoPosicionable = POKEMON;
    pokemonAtrapable->disponible = true;
    pokemonAtrapable->idMarcador = NULL;
    pthread_mutex_init(&pokemonAtrapable->mutexAtrapable, NULL);
    pokemonAtrapable->posicionInicial = parsearPosicion(posicionInicial, "|");
    pokemonAtrapable->posicion = &posicion;
    pokemonAtrapable->marcarComoObjetivo = &marcarComoObjetivo;
    pokemonAtrapable->esAtrapable = &esAtrapable;
    pokemonAtrapable->borrarPosicion = &borrarPosicion;
    pokemonAtrapable->descripcion = &descripcion;
    pokemonAtrapable->destruir = &destruirPokemonAtrapable;

    return pokemonAtrapable;
}

const struct PokemonAtrapableClass PokemonAtrapableConstructor = {.new=&new};
