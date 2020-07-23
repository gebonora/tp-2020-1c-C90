//
// Created by Alan Zhao on 15/07/2020.
//

#include "modelo/pokemon/CapturaPokemon.h"

static char * idEntrenador(CapturaPokemon * this) {
    return this->entrenador->id;
}

static char * especie(CapturaPokemon * this) {
    return this->pokemonAtrapable->especie;
}

static char * posicion(CapturaPokemon * this) {
    return coordenadaClave(this->pokemonAtrapable->posicionInicial);
}

static bool eliminarPokemonCapturadoDelMapa(CapturaPokemon * this) {
    return this->pokemonAtrapable->borrarPosicion(this->pokemonAtrapable);
}

static void destruirCapturaPokemon(CapturaPokemon * this) {
    log_destroy(this->logger);
    // El hecho de destruir una captura es porque dejó de existir el pokemon (ya sea por captura exitosa o no)
    this->pokemonAtrapable->destruir(this->pokemonAtrapable);
    free(this);
}

static CapturaPokemon * new(Entrenador * entrenador, uint32_t idCorrelatividad, PokemonAtrapable * pokemon) {
    CapturaPokemon * capturaPokemon = malloc(sizeof(CapturaPokemon));

    capturaPokemon->logger = log_create(TEAM_INTERNAL_LOG_FILE, "CapturaPokemon", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    capturaPokemon->entrenador = entrenador;
    capturaPokemon->idCorrelatividad = idCorrelatividad;
    capturaPokemon->pokemonAtrapable = pokemon;
    capturaPokemon->eliminarPokemonCapturadoDelMapa = &eliminarPokemonCapturadoDelMapa;
    capturaPokemon->posicion = &posicion;
    capturaPokemon->especie = &especie;
    capturaPokemon->idEntrenador = &idEntrenador;
    capturaPokemon->destruir = &destruirCapturaPokemon;

    return capturaPokemon;
}

const struct CapturaPokemonClass CapturaPokemonConstructor = {.new=&new};
