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
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B", "A|D|D");
    Entrenador * entrenador2 = EntrenadorConstructor.new("2|4", "A|B", "A|B");

    if (entrenador->objetivoCompletado(entrenador) != false || entrenador->puedeAtraparPokemones(entrenador) != true) {
        exit(EXIT_FAILURE);
    }

    if (entrenador2->objetivoCompletado(entrenador2) != true || entrenador2->puedeAtraparPokemones(entrenador2) != false) {
        exit(EXIT_FAILURE);
    }

    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenador);
    list_add(listaReady, hiloEntrenadorPlanificable);
    miAlgoritmo.proximoAEjecutar(&miAlgoritmo, listaReady);
    miAlgoritmo.destruir(&miAlgoritmo);
    entrenador->destruir(entrenador);
    entrenador2->destruir(entrenador2);
    list_destroy_and_destroy_elements(listaReady, (void (*)(void *)) hiloEntrenadorPlanificable->destruir);
}