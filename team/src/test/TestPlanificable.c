//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDePlanificable() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestPlanificable", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando la unidad planificable de los entrenadores");
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A", "B");
    Mapa mapita = MapaConstructor.new();
    registrarEnMapaPosicionEntrenador(&mapita, entrenador);
    HiloEntrenadorPlanificable * hiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenador);

    log_info(testLogger, "Testeando la ejecucion total de una tarea de captura");
    Coordinate posicionPokemon = (Coordinate){.pos_x=2, .pos_y=3};
    TareaPlanificable * tareaCaptura = generarTareaDeCaptura(hiloEntrenadorPlanificable->entrenador, "A", posicionPokemon);
    hiloEntrenadorPlanificable->asignarTarea(hiloEntrenadorPlanificable, tareaCaptura);
    hiloEntrenadorPlanificable->ejecutar(hiloEntrenadorPlanificable);

    assert(hiloEntrenadorPlanificable->tareaAsignada == NULL);

    log_info(testLogger, "Testeando el efecto de lado de la tarea de captura");
    Posicion posicionDeCaptura = entrenador->posicion(entrenador);
    assert(posicionDeCaptura.valida == true);
    assert(posicionDeCaptura.coordenada.pos_x == 2);
    assert(posicionDeCaptura.coordenada.pos_y == 3);

    log_info(testLogger, "Testeando la ejecucion parcial de una tarea de captura");
    Coordinate posicionPokemon2 = (Coordinate){.pos_x=0, .pos_y=1};
    TareaPlanificable * tareaCapturaParcial = generarTareaDeCaptura(hiloEntrenadorPlanificable->entrenador, "A", posicionPokemon2);
    hiloEntrenadorPlanificable->asignarTarea(hiloEntrenadorPlanificable, tareaCapturaParcial);
    hiloEntrenadorPlanificable->ejecutarParcialmente(hiloEntrenadorPlanificable, 1);

    assert(tareaCapturaParcial->estado == PENDIENTE_DE_EJECUCION);

    log_info(testLogger, "Testeando el efecto de lado de la tarea de captura parcialmente completada");

    Posicion posicionEnCaminoACapturar = entrenador->posicion(entrenador);
    assert(posicionEnCaminoACapturar.valida == true);
    assert(posicionEnCaminoACapturar.coordenada.pos_x == 1);
    assert(posicionEnCaminoACapturar.coordenada.pos_y == 3);

    log_info(testLogger, "Si ejecutamos por el remanente, la tarea deberia finalizar y eliminarse");
    hiloEntrenadorPlanificable->ejecutar(hiloEntrenadorPlanificable);

    assert(hiloEntrenadorPlanificable->tareaAsignada == NULL);
    posicionEnCaminoACapturar = entrenador->posicion(entrenador);
    assert(posicionEnCaminoACapturar.valida == true);
    assert(posicionEnCaminoACapturar.coordenada.pos_x == 0);
    assert(posicionEnCaminoACapturar.coordenada.pos_y == 1);

    hiloEntrenadorPlanificable->destruir(hiloEntrenadorPlanificable);
    entrenador->destruir(entrenador);
    mapita.destruir(&mapita);
    log_destroy(testLogger);
}
