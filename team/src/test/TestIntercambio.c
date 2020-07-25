//
// Created by Alan Zhao on 24/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeIntercambio(){
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestIntercambio", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando la tarea de intercambio y su efecto en los entrenadores");

    log_info(testLogger, "Sobreescribimos el equipo global temporalmente");
    Equipo equipoOriginal = equipoProcesoTeam;
    equipoProcesoTeam = list_create();

    Mapa mapa = MapaConstructor.new();
    Entrenador * entrenadorA = EntrenadorConstructor.new("1|1", "Bolvasor|Saidoc", "Bolvasor|Miutu");
    Entrenador * entrenadorB = EntrenadorConstructor.new("2|2", "Pichoto|Miutu", "Saidoc|Pichoto");
    registrarEnMapaPosicionEntrenador(&mapa, entrenadorA);
    registrarEnMapaPosicionEntrenador(&mapa, entrenadorB);
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificableA = HiloEntrenadorPlanificableConstructor.new(entrenadorA);

    list_add(equipoProcesoTeam, entrenadorA);
    list_add(equipoProcesoTeam, entrenadorB);

    TareaPlanificable * tareaDeIntercambio = generarTareaDeIntercambio(entrenadorA, entrenadorB, "Saidoc", "Miutu");
    entrenadorA->estaEsperandoAlgo = true;
    entrenadorB->estaEsperandoAlgo = true;

    log_info(testLogger, "Testeando la correcta generacion de la tarea");

    assert(tareaDeIntercambio->estado == PENDIENTE_DE_EJECUCION);

    log_info(testLogger, "Testeando la primera ejecucion de la tarea");

    hiloEntrenadorPlanificableA->asignarTarea(hiloEntrenadorPlanificableA, tareaDeIntercambio);
    hiloEntrenadorPlanificableA->ejecutarParcialmente(hiloEntrenadorPlanificableA, 1);

    log_info(testLogger, "Testeando que el entrenador movil haya dado un paso acercandose y el estatico se haya quedado en su lugar");

    assert(tareaDeIntercambio->estado == PENDIENTE_DE_EJECUCION);
    assert(entrenadorA->posicion(entrenadorA).coordenada.pos_x == 2);
    assert(entrenadorA->posicion(entrenadorA).coordenada.pos_y == 1);
    assert(entrenadorB->posicion(entrenadorB).coordenada.pos_x == 2);
    assert(entrenadorB->posicion(entrenadorB).coordenada.pos_y == 2);

    log_info(testLogger, "Testeando la segunda ejecucion de la tarea, moverse a la casilla del entrenador estatico");
    hiloEntrenadorPlanificableA->ejecutarParcialmente(hiloEntrenadorPlanificableA, 1);

    log_info(testLogger, "Testeando que el entrenador movil haya dado otro paso y el estatico siga en su lugar");
    assert(tareaDeIntercambio->estado == PENDIENTE_DE_EJECUCION);
    assert(entrenadorA->posicion(entrenadorA).coordenada.pos_x == 2);
    assert(entrenadorA->posicion(entrenadorA).coordenada.pos_y == 2);
    assert(entrenadorB->posicion(entrenadorB).coordenada.pos_x == 2);
    assert(entrenadorB->posicion(entrenadorB).coordenada.pos_y == 2);

    log_info(testLogger, "Testeando la ultima ejecucion de la tarea, el intercambio");
    hiloEntrenadorPlanificableA->ejecutarParcialmente(hiloEntrenadorPlanificableA, 1);

    log_info(testLogger, "Testeando que el intercambio haya impactado en la contabilidad de cada entrenador, cumpliendo el objetivo de ambos");
    assert(entrenadorA->objetivoCompletado(entrenadorA));
    assert(entrenadorA->objetivoCompletado(entrenadorB));

    log_info(testLogger, "Testeando que ambos entrenadores ya no esten pendientes de concluir la captura");
    assert(entrenadorA->estaEsperandoAlgo == false);
    assert(entrenadorB->estaEsperandoAlgo == false);

    log_info(testLogger, "Testeando que la tarea se haya consumido");

    assert(hiloEntrenadorPlanificableA->tareaAsignada == NULL);

    hiloEntrenadorPlanificableA->destruir(hiloEntrenadorPlanificableA);
    mapa.destruir(&mapa);
    log_info(testLogger, "Restauro el equipo global original");
    destruirEquipo(equipoProcesoTeam);
    equipoProcesoTeam = equipoOriginal;
    log_destroy(testLogger);
}