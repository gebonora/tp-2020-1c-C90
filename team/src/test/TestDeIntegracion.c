//
// Created by Alan Zhao on 10/06/2020.
//

#include "test/TestDeIntegracion.h"

void testDeIntegracion() {
    testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestsDeIntegracion", 1, LOG_LEVEL_INFO);

    // Entrenadores
    testDeEntrenadores();

    // Movimiento
    testDeMovimiento();

    // Mapa
    testDeMapa();

    // Algoritmos
    testDeAlgoritmos();

    // Tareas
    testDeTareas();

    // Planificacion
    testDePlanificable();

    // Eventos
    testDeEventos();

    log_info(testLogger, "Pruebas de integracion finalizadas con exito");
    log_destroy(testLogger);
}