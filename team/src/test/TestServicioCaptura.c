//
// Created by Alan Zhao on 15/07/2020.
//

#include "test/TestDeIntegracion.h"

void testServicioDeCaptura() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestServicioCaptura", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando el servicio de captura");
    Mapa mapa = MapaConstructor.new();
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "", "");
    free(entrenador->id);
    entrenador->id = string_duplicate("EntrenadorPiola");
    registrarEnMapaPosicionEntrenador(&mapa, entrenador);
    Equipo equipo = list_create();
    list_add(equipo, entrenador);

    ServicioDePlanificacion * servicioPlanificacion = ServicioDePlanificacionConstructor.new();
    ServicioDeCaptura * servicioDeCaptura = ServicioDeCapturaConstructor.new(mapa, equipo, servicioPlanificacion);

    log_info(testLogger, "Testeando la aparicion de un pokemon");
    Coordinate posicion = {.pos_x = 1, .pos_y = 2};
    log_info(testLogger, "No deberia poder capturarse porque el unico entrenador no tiene espacio");
    servicioDeCaptura->procesarPokemonCapturable(servicioDeCaptura, "MissigNO", posicion);

    log_info(testLogger, "Testeando que en el mapa exista 1 entrenador y 1 pokemon");
    //assert(mapa.cantidadPosicionables(&mapa) == 2);
    //TODO: Sacamos el uuid de la lista de pokemones del servicio y matcheamos en el mapa las coordenadas

    log_info(testLogger, "Testeando el impacto de una captura exitosa");
    PokemonAtrapable * pokemonAtrapable = PokemonAtrapableConstructor.new("Pikachu", "1|2");
    char * uuidPokemon = string_duplicate(registrarEnMapaPosicionPokemonAtrapable(&mapa, pokemonAtrapable));
    CapturaPokemon * capturaPokemon = CapturaPokemonConstructor.new("EntrenadorPiola", "1", pokemonAtrapable);

    log_info(testLogger, "Testeando que se haya podido concretar la captura");
    bool resultadoCaptura = servicioDeCaptura->registrarCapturaExitosa(servicioDeCaptura, capturaPokemon);

    assert(resultadoCaptura == true);

    log_info(testLogger, "Testeando que la captura haya volado del mapa al pokemon y solo quede 1 entrenador");
    Posicion posicionPokemon = mapa.obtenerPosicion(&mapa, uuidPokemon);

    assert(posicionPokemon.valida == false);
    //assert(mapa.cantidadPosicionables(&mapa) == 1);

    //pokemonAtrapable->destruir(pokemonAtrapable);
    capturaPokemon->destruir(capturaPokemon);
    servicioDeCaptura->destruir(servicioDeCaptura);
    servicioPlanificacion->destruir(servicioPlanificacion);
    mapa.destruir(&mapa);
    destruirEquipo(equipo);
    free(uuidPokemon);
    log_destroy(testLogger);
}