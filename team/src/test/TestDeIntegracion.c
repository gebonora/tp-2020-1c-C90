//
// Created by Alan Zhao on 10/06/2020.
//

#include "test/TestDeIntegracion.h"

void testDeIntegracion() {

    // Entrenadores
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B", "A|D|D");
    Entrenador * entrenador2 = EntrenadorConstructor.new("2|4", "A|B", "A|B");

    assert(entrenador->objetivoCompletado(entrenador) == false);
    assert(entrenador->puedeAtraparPokemones(entrenador) == true);
    assert(entrenador2->objetivoCompletado(entrenador2) == true);
    assert(entrenador2->puedeAtraparPokemones(entrenador2) == false);

    entrenador->destruir(entrenador);
    entrenador2->destruir(entrenador2);

    // Mapa
    Mapa mapita = MapaConstructor.new();
    Entrenador * entrenadorARegistrar = EntrenadorConstructor.new("3|3", "A", "B");

    registrarEnMapaPosicionEntrenador(&mapita, entrenadorARegistrar);

    Casilla casilla = dictionary_get(mapita.plano, "(3,3)");
    Presencia * presencia = list_get(casilla, 0);

    assert(entrenadorARegistrar->uuid == presencia->uuid);

    entrenadorARegistrar->destruir(entrenadorARegistrar);
    mapita.destruir(&mapita);

    // Planificacion
    AlgoritmoPlanificador miAlgoritmo = obtenerAlgoritmo("FIFO");
    t_list * listaReady = list_create();
    Entrenador * entrenadorPiola = EntrenadorConstructor.new("1|2", "A|B", "A|D|D");
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenadorPiola);
    list_add(listaReady, hiloEntrenadorPlanificable);
    miAlgoritmo.proximoAEjecutar(&miAlgoritmo, listaReady);
    miAlgoritmo.destruir(&miAlgoritmo);
    entrenadorPiola->destruir(entrenadorPiola);
    list_destroy_and_destroy_elements(listaReady, (void (*)(void *)) hiloEntrenadorPlanificable->destruir);

    // Eventos
    manejadorDeEventos = ManejadorDeEventosConstructor.new();
    manejadorDeEventos.notificarEventoLocalized(&manejadorDeEventos);
    manejadorDeEventos.destruir(&manejadorDeEventos);
}