//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeMapa() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestMapa", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando la creacion del mapa");
    Mapa mapita = MapaConstructor.new();

    log_info(testLogger, "Testeando el registro de un entrenador");
    Entrenador * entrenadorARegistrar = EntrenadorConstructor.new("3|3", "A", "B");
    char * uuidRegistroEntrenador = registrarEnMapaPosicionEntrenador(&mapita, entrenadorARegistrar);
    Posicion posicionDelEntrenadorRegistrado = entrenadorARegistrar->posicion(entrenadorARegistrar);

    assert(string_equals(uuidRegistroEntrenador, entrenadorARegistrar->gps->uuid));
    assert(posicionDelEntrenadorRegistrado.valida == true);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_x == 3);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_y == 3);

    log_info(testLogger, "Testeando el registro de un pokemon");
    PokemonAtrapable * pokemonARegistrar = PokemonAtrapableConstructor.new("Pikachu", "2|2");
    char * uuidRegistroPokemon = registrarEnMapaPosicionPokemonAtrapable(&mapita, pokemonARegistrar);
    Posicion posicionDelPokemonRegistrado = pokemonARegistrar->posicion(pokemonARegistrar);

    assert(string_equals(uuidRegistroPokemon, pokemonARegistrar->gps->uuid));
    assert(posicionDelPokemonRegistrado.valida == true);
    assert(posicionDelPokemonRegistrado.coordenada.pos_x == 2);
    assert(posicionDelPokemonRegistrado.coordenada.pos_y == 2);

    log_info(testLogger, "Testeando que los uuid del entrenador y el pokemon sean distintos");
    assert(!string_equals(uuidRegistroEntrenador, uuidRegistroPokemon));

    log_info(testLogger, "Testeando el movimiento de un entrenador");
    Coordinate * destino = create_coordinate(3,4);
    entrenadorARegistrar->mover(entrenadorARegistrar, destino);
    posicionDelEntrenadorRegistrado = entrenadorARegistrar->posicion(entrenadorARegistrar);

    assert(posicionDelEntrenadorRegistrado.valida == true);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_x == 3);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_y == 4);

    log_info(testLogger, "Testeando el conteo de posicionables en el mapa");
    assert(mapita.cantidadPosicionables(&mapita) == 2);

    free(destino);
    entrenadorARegistrar->destruir(entrenadorARegistrar);
    pokemonARegistrar->destruir(pokemonARegistrar);
    mapita.destruir(&mapita);
    log_destroy(testLogger);
}
