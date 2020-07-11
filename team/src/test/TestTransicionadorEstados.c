//
// Created by Alan Zhao on 11/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeTransicionadorDeEstados() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestTransicionadorEstados", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando el transicionador de estados");

    log_destroy(testLogger);
}