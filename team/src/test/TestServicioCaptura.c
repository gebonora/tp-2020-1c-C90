//
// Created by Alan Zhao on 15/07/2020.
//

#include "test/TestDeIntegracion.h"

void testServicioDeCaptura() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestServicioCaptura", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando el servicio de captura");
    Mapa mapa = MapaConstructor.new();
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B", "A|C|C");
    free(entrenador->id);
    entrenador->id = string_from_format("EntrenadorPiola");
    Equipo equipo = list_create();
    list_add(equipo, entrenador);
    PokemonAtrapable * pokemonAtrapable = PokemonAtrapableConstructor.new("Pikachu", "1|2");
    char * uuidPokemon = registrarEnMapaPosicionPokemonAtrapable(&mapa, pokemonAtrapable);
    ServicioDePlanificacion * servicioPlanificacion = ServicioDePlanificacionConstructor.new();
    ServicioDeCaptura * servicioDeCaptura = ServicioDeCapturaConstructor.new(mapa, equipo, servicioPlanificacion);

    log_info(testLogger, "Testeando el impacto de una captura exitosa");

    assert(mapa.cantidadPosicionables(&mapa) == 1);

    CapturaPokemon * capturaPokemon = CapturaPokemonConstructor.new("EntrenadorPiola", "1", pokemonAtrapable);

    bool resultadoCaptura = servicioDeCaptura->registrarCapturaExitosa(servicioDeCaptura, capturaPokemon);

    assert(resultadoCaptura == true);

    Posicion posicionPokemon = mapa.obtenerPosicion(&mapa, uuidPokemon);

    assert(posicionPokemon.valida == false);

    assert(mapa.cantidadPosicionables(&mapa) == 0);

    capturaPokemon->destruir(capturaPokemon);
    servicioDeCaptura->destruir(servicioDeCaptura);
    servicioPlanificacion->destruir(servicioPlanificacion);
    mapa.destruir(&mapa);
    destruirEquipo(equipo);
    log_destroy(testLogger);
}