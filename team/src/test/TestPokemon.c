//
// Created by Alan Zhao on 15/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDePokemones() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestPokemon", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando el comportamiento de los pokemones");
    //TODO Clavar algunos tests
    PokemonAtrapable * pokemon = PokemonAtrapableConstructor.new("PIKACHU", "1|3");

    pokemon->destruir(pokemon);
    log_destroy(testLogger);
}