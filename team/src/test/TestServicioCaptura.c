//
// Created by Alan Zhao on 15/07/2020.
//

#include "test/TestDeIntegracion.h"

void testServicioDeCaptura() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestServicioCaptura", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando el servicio de captura");
    Mapa mapa = MapaConstructor.new();
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "", "");
    registrarEnMapaPosicionEntrenador(&mapa, entrenador);
    Equipo equipo = list_create();
    list_add(equipo, entrenador);

    ServicioDeMetricas* metricasTest = ServicioDeMetricasConstructor.new();
    ServicioDeResolucionDeDeadlocks* deadlocksTest = ServicioDeResolucionDeDeadlocksConstructor.new(metricasTest);
    ServicioDePlanificacion * servicioPlanificacion = ServicioDePlanificacionConstructor.new(metricasTest, deadlocksTest);
    ServicioDeCaptura * servicioDeCaptura = ServicioDeCapturaConstructor.new(mapa, servicioPlanificacion);

    log_info(testLogger, "Testeando la aparicion de un pokemon");
    Coordinate posicion = {.pos_x = 1, .pos_y = 2};
    log_info(testLogger, "No deberia poder encargarse la captura porque el unico entrenador no tiene espacio");
    servicioDeCaptura->procesarPokemonCapturable(servicioDeCaptura, "MissigNO", posicion);

    log_info(testLogger, "Testeando que en el mapa exista 1 entrenador y 1 pokemon");
    assert(mapa.cantidadPosicionables(&mapa) == 2);

    log_info(testLogger, "Testeando el impacto de una captura exitosa");
    PokemonAtrapable * pokemonAtrapable = PokemonAtrapableConstructor.new("Pikachu", "1|2");
    char * uuidPokemon = string_duplicate(registrarEnMapaPosicionPokemonAtrapable(&mapa, pokemonAtrapable));
    CapturaPokemon * capturaPokemon = CapturaPokemonConstructor.new(entrenador, 1, pokemonAtrapable);

    log_info(testLogger, "Testeando que se haya podido concretar la captura");
    bool resultadoCaptura = servicioDeCaptura->registrarCapturaExitosa(servicioDeCaptura, capturaPokemon);

    assert(resultadoCaptura == true);

    log_info(testLogger, "Testeando que la captura haya volado del mapa al pokemon y solo quede 1 entrenador");
    Posicion posicionPokemon = mapa.obtenerPosicion(&mapa, uuidPokemon);

    assert(posicionPokemon.valida == false);
    //assert(mapa.cantidadPosicionables(&mapa) == 1);

    servicioDeCaptura->destruir(servicioDeCaptura);
    servicioPlanificacion->destruir(servicioPlanificacion);
    metricasTest->destruir(metricasTest);
    deadlocksTest->destruir(deadlocksTest);
    mapa.destruir(&mapa);
    destruirEquipo(equipo);
    free(uuidPokemon);
    log_destroy(testLogger);
}
