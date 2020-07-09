//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeEventos() {
    log_info(testLogger, "Testeando los eventos del sistema");
    ManejadorDeEventos manejadorDeEventosTest = ManejadorDeEventosConstructor.new();
    manejadorDeEventosTest.notificarEventoLocalized(&manejadorDeEventosTest);
    manejadorDeEventosTest.destruir(&manejadorDeEventosTest);
}