//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeEventos() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestEventos", 1, LOG_LEVEL_INFO);
    log_info(testLogger, "Testeando los eventos del sistema");
    ManejadorDeEventos manejadorDeEventosTest = ManejadorDeEventosConstructor.new();
    //manejadorDeEventosTest.notificarNuevoPokemon(&manejadorDeEventosTest);
    manejadorDeEventosTest.destruir(&manejadorDeEventosTest);
    log_destroy(testLogger);
}
