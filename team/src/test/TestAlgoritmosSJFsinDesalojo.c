/*
 * TestAlgoritmosSJFsinDesalojo.c
 *
 *  Created on: 17 jul. 2020
 *      Author: utnso
 */

#include "test/TestDeIntegracion.h"


//TODO: realizar el test para probar que funcione como queremos que lo haga

void testDeAlgoritmosSJFsinDesalojo() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestAlgoritmoSJFsinDesalojo", 1, LOG_LEVEL_INFO);

    t_list * listaReady = list_create();

    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B", "A|C");
    Entrenador * entrenador2 = EntrenadorConstructor.new("4|4", "C", "D");

    char * idEntrenadorQueDeberiaSerSeleccionado = entrenador2->id;

    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenador);
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable2 = HiloEntrenadorPlanificableConstructor.new(entrenador2);

    hiloEntrenadorPlanificable->infoUltimaEjecucion.primeraEstimacion = true;
    hiloEntrenadorPlanificable->infoUltimaEjecucion.real_raf_ant = 10;
    hiloEntrenadorPlanificable->infoUltimaEjecucion.est_raf_ant = 0;
    hiloEntrenadorPlanificable->infoUltimaEjecucion.est_raf_actual = 5;


    hiloEntrenadorPlanificable2->infoUltimaEjecucion.primeraEstimacion = true;
    hiloEntrenadorPlanificable2->infoUltimaEjecucion.real_raf_ant = 1;
    hiloEntrenadorPlanificable2->infoUltimaEjecucion.est_raf_ant = 0;
    hiloEntrenadorPlanificable2->infoUltimaEjecucion.est_raf_actual = 1;

    list_add(listaReady, hiloEntrenadorPlanificable);
    list_add(listaReady, hiloEntrenadorPlanificable2);


    //SJF - SD
    log_info(testLogger, "Testeando SFJ-SD");
    AlgoritmoPlanificador miAlgoritmo = obtenerAlgoritmo("SJF-SD");
    HiloEntrenadorPlanificable * planificableSeleccioado = miAlgoritmo.proximoAEjecutar(&miAlgoritmo, listaReady);
    assert(string_equals(idEntrenadorQueDeberiaSerSeleccionado, planificableSeleccioado->entrenador->id));

    list_destroy_and_destroy_elements(listaReady, (void (*)(void *)) hiloEntrenadorPlanificable->destruir);
    entrenador->destruir(entrenador);
    entrenador2->destruir(entrenador2);

    log_destroy(testLogger);
}
