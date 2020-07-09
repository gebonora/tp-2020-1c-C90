//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"


void testDeAlgoritmos() {
    log_info(testLogger, "Testeando los algoritmos de planificacion");
    t_list * listaReady = list_create();
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B", "A|C");
    Entrenador * entrenador2 = EntrenadorConstructor.new("3|4", "C", "D");
    char * idEntrenadorQueDeberiaSerSeleccionado = entrenador->id;
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenador);
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable2 = HiloEntrenadorPlanificableConstructor.new(entrenador2);
    list_add(listaReady, hiloEntrenadorPlanificable);
    list_add(listaReady, hiloEntrenadorPlanificable2);

    //FIFO
    log_info(testLogger, "Testeando FIFO");
    AlgoritmoPlanificador miAlgoritmo = obtenerAlgoritmo("FIFO");
    HiloEntrenadorPlanificable * planificableSeleccioado = miAlgoritmo.proximoAEjecutar(&miAlgoritmo, listaReady);
    assert(string_equals(idEntrenadorQueDeberiaSerSeleccionado, planificableSeleccioado->entrenador->id));
    miAlgoritmo.destruir(&miAlgoritmo);

    list_destroy_and_destroy_elements(listaReady, (void (*)(void *)) hiloEntrenadorPlanificable->destruir);
    entrenador->destruir(entrenador);
    entrenador2->destruir(entrenador2);
}