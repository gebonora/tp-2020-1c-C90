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

void testDeMovimiento() {
    log_info(testLogger, "Testeando la correcta generacion de caminos entre coordenadas");
    Coordinate origen = {.pos_x = 1, .pos_y = 3};
    Coordinate destino = {.pos_x = 5, .pos_y = 2};

    Camino camino = generarCamino(origen, destino);

    assert(list_size(camino) == 5); // [(2,3), (3,3), (4,3), (5,3), (5,2)]

    Coordinate * paso1 = list_get(camino, 0);
    Coordinate * paso2 = list_get(camino, 1);
    Coordinate * paso3 = list_get(camino, 2);
    Coordinate * paso4 = list_get(camino, 3);
    Coordinate * paso5 = list_get(camino, 4);

    assert(paso1->pos_x == 2 && paso1->pos_y == 3);
    assert(paso2->pos_x == 3 && paso2->pos_y == 3);
    assert(paso3->pos_x == 4 && paso3->pos_y == 3);
    assert(paso4->pos_x == 5 && paso4->pos_y == 3);
    assert(paso5->pos_x == 5 && paso5->pos_y == 2);

    list_destroy_and_destroy_elements(camino, free);
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

    log_info(testLogger, "Testeando el movimiento de un entrenador");
    Coordinate * destino = create_coordinate(3,4);
    entrenadorARegistrar->mover(entrenadorARegistrar, destino);
    posicionDelEntrenadorRegistrado = entrenadorARegistrar->posicion(entrenadorARegistrar);

    assert(posicionDelEntrenadorRegistrado.valida == true);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_x == 3);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_y == 4);

    free(destino);
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

void testDeTareas() {
    log_info(testLogger, "Testeando la generacion de una tarea de captura");
    Entrenador * entrenador = EntrenadorConstructor.new("4|20", "A", "B");
    Mapa mapita = MapaConstructor.new();
    registrarEnMapaPosicionEntrenador(&mapita, entrenador);
    Coordinate posicionPokemon = (Coordinate){.pos_x=6, .pos_y=20};
    TareaPlanificable * tareaCaptura = generarTareaDeCaptura(entrenador, "A", posicionPokemon);

    assert(list_size(tareaCaptura->instrucciones) == 2);
    assert(tareaCaptura->totalInstrucciones == 2);
    assert(tareaCaptura->estado == PENDIENTE_DE_EJECUCION);

    Instruccion * instruccion1 = tareaCaptura->proximaInstruccion(tareaCaptura);
    assert(tareaCaptura->estado == EJECUTANDO);

    tareaCaptura->notificarEjecucion(tareaCaptura, 0);
    assert(tareaCaptura->estado == PENDIENTE_DE_EJECUCION);

    Instruccion * instruccion2 = tareaCaptura->proximaInstruccion(tareaCaptura);
    assert(tareaCaptura->estado == EJECUTANDO);

    tareaCaptura->notificarEjecucion(tareaCaptura, 1);
    assert(tareaCaptura->estado == FINALIZADA);
    assert(tareaCaptura->contadorDeInstrucciones + 1 == tareaCaptura->totalInstrucciones);

    assert(tareaCaptura->proximaInstruccion(tareaCaptura) == NULL);
    assert(tareaCaptura->estado == FINALIZADA);

    tareaCaptura->notificarEjecucion(tareaCaptura, 1203);
    assert(tareaCaptura->estado == ABORTADA);

    assert(tareaCaptura->proximaInstruccion(tareaCaptura) == NULL);
    assert(tareaCaptura->estado == ABORTADA);

    tareaCaptura->destruir(tareaCaptura);
    entrenador->destruir(entrenador);
    mapita.destruir(&mapita);
}

void testDePlanificable() {
    log_info(testLogger, "Testeando la unidad planificable de los entrenadores");
    Entrenador * entrenador = EntrenadorConstructor.new("7|3", "A", "B");
    Mapa mapita = MapaConstructor.new();
    registrarEnMapaPosicionEntrenador(&mapita, entrenador);
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenador);

    log_info(testLogger, "Testeando la ejecucion total de una tarea de captura");
    Coordinate posicionPokemon = (Coordinate){.pos_x=3, .pos_y=5};
    TareaPlanificable * tareaCaptura = generarTareaDeCaptura(hiloEntrenadorPlanificable->entrenador, "A", posicionPokemon);

    hiloEntrenadorPlanificable->ejecutar(hiloEntrenadorPlanificable, tareaCaptura);


    assert(tareaCaptura->estado == FINALIZADA);

    log_info(testLogger, "Testeando el efecto de lado de la tarea de captura");
    Posicion posicionDeCaptura = entrenador->posicion(entrenador);
    assert(posicionDeCaptura.valida == true);
    assert(posicionDeCaptura.coordenada.pos_x == 3);
    assert(posicionDeCaptura.coordenada.pos_y == 5);

    tareaCaptura->destruir(tareaCaptura);
    hiloEntrenadorPlanificable->destruir(hiloEntrenadorPlanificable);
    entrenador->destruir(entrenador);
    mapita.destruir(&mapita);
}

void testDeEventos() {
    log_info(testLogger, "Testeando los eventos del sistema");
    ManejadorDeEventos manejadorDeEventosTest = ManejadorDeEventosConstructor.new();
    manejadorDeEventosTest.notificarEventoLocalized(&manejadorDeEventosTest);
    manejadorDeEventosTest.destruir(&manejadorDeEventosTest);
}