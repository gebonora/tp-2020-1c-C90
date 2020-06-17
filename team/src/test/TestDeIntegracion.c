//
// Created by Alan Zhao on 10/06/2020.
//

#include "test/TestDeIntegracion.h"

void testDeIntegracion() {
    manejadorDeEventos = ManejadorDeEventosConstructor.new();
    manejadorDeEventos.notificarEventoLocalized(&manejadorDeEventos);
    manejadorDeEventos.destruir(&manejadorDeEventos);

    AlgoritmoPlanificador miAlgoritmo = obtenerAlgoritmo("FIFO");
    t_list * listaReady = list_create();
    Entrenador * entrenador = EntrenadorConstructor.new("1|2");
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenador);
    list_add(listaReady, hiloEntrenadorPlanificable);
    miAlgoritmo.proximoAEjecutar(&miAlgoritmo, listaReady);
    miAlgoritmo.destruir(&miAlgoritmo);
    entrenador->destruir(entrenador);
    list_destroy_and_destroy_elements(listaReady, (void (*)(void *)) hiloEntrenadorPlanificable->destruir);
}