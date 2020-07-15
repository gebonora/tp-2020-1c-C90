//
// Created by Alan Zhao on 15/07/2020.
//

#include "test/TestDeIntegracion.h"

void testServicioDeCaptura() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestServicioCaptura", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando el servicio de captura");
    Mapa mapa = MapaConstructor.new();
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B", "A|C|C");
    Equipo equipo = list_create();
    list_add(equipo, entrenador);
    ServicioDePlanificacion * servicioPlanificacion = ServicioDePlanificacionConstructor.new();
    ServicioDeCaptura * servicioDeCaptura = ServicioDeCapturaConstructor.new(mapa, equipo, servicioPlanificacion);

    //TODO Clavar algunos tests

    servicioDeCaptura->destruir(servicioDeCaptura);
    servicioPlanificacion->destruir(servicioPlanificacion);
    mapa.destruir(&mapa);
    destruirEquipo(equipo);
    log_destroy(testLogger);
}