//
// Created by Alan Zhao on 15/07/2020.
//

#include "modelo/pokemon/CapturaPokemon.h"

static bool eliminarPokemonCapturadoDelMapa(CapturaPokemon * this, Mapa mapa) {
    return this->pokemonAtrapable->borrarPosicion(this->pokemonAtrapable);
}

static void destruirCapturaPokemon(CapturaPokemon * this) {
    log_destroy(this->logger);
    this->pokemonAtrapable->destruir(this->pokemonAtrapable);
    free(this);
}

static CapturaPokemon * new(char * idEntrenador, char * idCorrelatividad, PokemonAtrapable * pokemon) {
    CapturaPokemon * capturaPokemon = malloc(sizeof(CapturaPokemon));

    capturaPokemon->logger = log_create(TEAM_INTERNAL_LOG_FILE, "CapturaPokemon", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    capturaPokemon->idEntrenador = idEntrenador;
    capturaPokemon->idCorrelatividad = idCorrelatividad;
    capturaPokemon->eliminarPokemonCapturadoDelMapa = &eliminarPokemonCapturadoDelMapa;
    capturaPokemon->destruir = &destruirCapturaPokemon;

    return capturaPokemon;
}

const struct CapturaPokemonClass CapturaPokemonConstructor = {.new=&new};