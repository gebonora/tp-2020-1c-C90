//
// Created by Alan Zhao on 10/06/2020.
//

#include "test/TestDeIntegracion.h"

void testDeIntegracion() {
    testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestsDeIntegracion", 1, LOG_LEVEL_INFO);

    // Entrenadores
    testDeEntrenadores();

    // Mapa
    testDeMapa();

    // Algoritmos
    testDeAlgoritmos();

    // Planificacion
    testDePlanificable();

    // Eventos
    testDeEventos();

    log_info(testLogger, "Pruebas de integracion finalizadas con exito");
    log_destroy(testLogger);
}

void testDeEntrenadores() {
    log_info(testLogger, "Testeando los metodos de los entrenadores");
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B", "A|C|C");
    Entrenador * entrenador2 = EntrenadorConstructor.new("2|4", "A|B", "A|B");

    assert(entrenador->objetivoCompletado(entrenador) == false);
    assert(entrenador->puedeAtraparPokemones(entrenador) == true);
    assert(entrenador2->objetivoCompletado(entrenador2) == true);
    assert(entrenador2->puedeAtraparPokemones(entrenador2) == false);
    assert((int) dictionary_get(entrenador->pokemonesCapturados, "A") == 1);
    assert((int) dictionary_get(entrenador->pokemonesCapturados, "B") == 1);
    assert((int) dictionary_get(entrenador->pokemonesObjetivo, "A") == 1);
    assert((int) dictionary_get(entrenador->pokemonesObjetivo, "C") == 2);
    assert((int) dictionary_get(entrenador2->pokemonesCapturados, "A") == 1);
    assert((int) dictionary_get(entrenador2->pokemonesCapturados, "B") == 1);
    assert((int) dictionary_get(entrenador2->pokemonesObjetivo, "A") == 1);
    assert((int) dictionary_get(entrenador2->pokemonesObjetivo, "B") == 1);

    Equipo equipito = list_create();
    list_add(equipito, entrenador);
    list_add(equipito, entrenador2);

    log_info(testLogger, "Testeando los metodos del objetivo global");
    ObjetivoGlobal objetivo = ObjetivoGlobalConstructor.new(equipito);

    t_list * especiesNecesarias = objetivo.especiesNecesarias(&objetivo);

    ContabilidadEspecie * contabilidadEspecieA = dictionary_get(objetivo.contabilidadEspecies, "A");
    ContabilidadEspecie * contabilidadEspecieB = dictionary_get(objetivo.contabilidadEspecies, "B");
    ContabilidadEspecie * contabilidadEspecieC = dictionary_get(objetivo.contabilidadEspecies, "C");

    assert(list_size(especiesNecesarias) == 3);
    assert(string_equals(list_get(especiesNecesarias, 0),"A"));
    assert(string_equals(list_get(especiesNecesarias, 1),"B"));
    assert(string_equals(list_get(especiesNecesarias, 2),"C"));
    assert(objetivo.puedeCapturarse(&objetivo, "A") == false);
    assert(objetivo.puedeCapturarse(&objetivo, "B") == false);
    assert(objetivo.puedeCapturarse(&objetivo, "C") == true);
    assert(objetivo.puedeCapturarse(&objetivo, "X") == false);
    assert(contabilidadEspecieA->necesarios == 2);
    assert(contabilidadEspecieA->capturados == 2);
    assert(contabilidadEspecieB->necesarios == 1);
    assert(contabilidadEspecieB->capturados == 2);
    assert(contabilidadEspecieC->necesarios == 2);
    assert(contabilidadEspecieC->capturados == 0);

    list_destroy(especiesNecesarias);
    destruirEquipo(equipito);
    objetivo.destruirObjetivoGlobal(&objetivo);
}

void testDeMapa() {
    log_info(testLogger, "Testeando la creacion del mapa");
    Mapa mapita = MapaConstructor.new();
    log_info(testLogger, "Testeando el registro de posicionables en el mapa");
    Entrenador * entrenadorARegistrar = EntrenadorConstructor.new("3|3", "A", "B");

    char * uuidRegistro = registrarEnMapaPosicionEntrenador(&mapita, entrenadorARegistrar);
    Posicion posicionDelEntrenadorRegistrado = entrenadorARegistrar->posicion(entrenadorARegistrar);

    assert(string_equals(uuidRegistro, entrenadorARegistrar->gps->uuid));
    assert(posicionDelEntrenadorRegistrado.valida == true);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_x == 3);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_y == 3);

    entrenadorARegistrar->destruir(entrenadorARegistrar);
    mapita.destruir(&mapita);
}

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

void testDePlanificable() {
    log_info(testLogger, "Testeando la unidad planificable de los entrenadores");
    Entrenador * entrenador = EntrenadorConstructor.new("7|3", "A", "B");
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenador);

    //TODO: Meterle una tarea y que la haga de a poco.

    hiloEntrenadorPlanificable->destruir(hiloEntrenadorPlanificable);
    entrenador->destruir(entrenador);
}

void testDeEventos() {
    log_info(testLogger, "Testeando los eventos del sistema");
    ManejadorDeEventos manejadorDeEventosTest = ManejadorDeEventosConstructor.new();
    manejadorDeEventosTest.notificarEventoLocalized(&manejadorDeEventosTest);
    manejadorDeEventosTest.destruir(&manejadorDeEventosTest);
}