//
// Created by Alan Zhao on 11/07/2020.
//

#include "test/TestDeIntegracion.h"

void testServicioDePlanificacion() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestServicioPlanificacion", 1, LOG_LEVEL_INFO);

    Equipo miEquipo = list_create();
    Entrenador * entrenador1 = EntrenadorConstructor.new("0|0", "A", "B");
    list_add(miEquipo, entrenador1);

    ServicioDePlanificacion * servicio = ServicioDePlanificacionConstructor.new(miEquipo);

    //TODO: Tests del servicio de planificacion

    servicio->destruir(servicio);
    destruirEquipo(miEquipo);
    log_destroy(testLogger);
}