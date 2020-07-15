//
// Created by Alan Zhao on 15/07/2020.
//

#include "modelo/pokemon/PokemonAtrapable.h"

static Posicion posicion(PokemonAtrapable * this) {
    Gps * gps = this->gps;
    if (gps == NULL) {
        log_error(this->logger, "El pokemon no tiene un GPS asignado. No puede responder su ubicación.");
        return (Posicion) {.valida = false};
    }
    return gps->posicionActual(gps);
}

static void destruirPokemonAtrapable(PokemonAtrapable * this) {
    Posicion posicion = this->posicion(this);
    if (posicion.valida) {
        char * coordenada = coordenadaClave(posicion.coordenada);
        log_debug(this->logger, "Se procede a destruir un %s en %s", this->especie, coordenada);
        free(coordenada);
    } else {
        char * posicionInicial = coordenadaClave(this->posicionInicial);
        log_debug(this->logger, "Se procede a destruir un %s que debería estar en %s, pero nunca se registró.", this->especie, posicionInicial);
        free(posicionInicial);
    }
    log_destroy(this->logger);
    if (this->gps) {
        this->gps->destruirGps(this->gps);
    }
    free(this);
}

static PokemonAtrapable * new(char * especie, char * posicionInicial) {
    PokemonAtrapable * pokemonAtrapable = malloc(sizeof(PokemonAtrapable));

    pokemonAtrapable->logger = log_create(TEAM_INTERNAL_LOG_FILE, "PokemonAtrapable", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    pokemonAtrapable->especie = especie;
    pokemonAtrapable->gps = NULL;
    pokemonAtrapable->tipoPosicionable = POKEMON;
    pokemonAtrapable->posicionInicial = parsearPosicion(posicionInicial, "|");
    pokemonAtrapable->posicion = &posicion;
    pokemonAtrapable->destruir = &destruirPokemonAtrapable;

    return pokemonAtrapable;
}

const struct PokemonAtrapableClass PokemonAtrapableConstructor = {.new=&new};
