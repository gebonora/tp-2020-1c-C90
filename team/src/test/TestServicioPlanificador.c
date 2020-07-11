//
// Created by Alan Zhao on 11/07/2020.
//

#include "test/TestDeIntegracion.h"

void testServicioDePlanificacion() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestServicioPlanificacion", 1, LOG_LEVEL_INFO);

    ServicioDePlanificacion * servicio = ServicioDePlanificacionConstructor.new();

    servicio->destruir(servicio);
    log_destroy(testLogger);
}