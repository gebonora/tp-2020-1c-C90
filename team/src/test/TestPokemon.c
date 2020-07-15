//
// Created by Alan Zhao on 15/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDePokemones() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestPokemon", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando el comportamiento de los pokemones");

    Mapa mapa = MapaConstructor.new();
    PokemonAtrapable * pokemon = PokemonAtrapableConstructor.new("PIKACHU", "1|3");
    char * uuid = registrarEnMapaPosicionPokemonAtrapable(&mapa, pokemon);

    assert(pokemon->posicionInicial.pos_x == 1);
    assert(pokemon->posicionInicial.pos_y == 3);
    assert(pokemon->esAtrapable(pokemon) == true);
    assert(pokemon->marcarComoObjetivo(pokemon, "EntrenadorRapido") == true);
    assert(pokemon->esAtrapable(pokemon) == false);
    assert(pokemon->marcarComoObjetivo(pokemon, "EntrenadorLento") == false);

    Posicion posicion = pokemon->posicion(pokemon);
    assert(posicion.valida == true);
    assert(posicion.coordenada.pos_x = 1);
    assert(posicion.coordenada.pos_y = 3);

    assert(pokemon->borrarPosicion(pokemon) == true);

    Posicion posicionLuegoDeBorrar = pokemon->posicion(pokemon);
    assert(posicionLuegoDeBorrar.valida == false);

    Posicion posicionEnMapaLuegoDeBorrar = mapa.obtenerPosicion(&mapa, uuid);
    //TODO: assert(posicionEnMapaLuegoDeBorrar.valida == false);

    pokemon->destruir(pokemon);
    mapa.destruir(&mapa);
    log_destroy(testLogger);
}