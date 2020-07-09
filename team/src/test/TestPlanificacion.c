//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

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

    log_info(testLogger, "Testeando la ejecucion parcial de una tarea de captura");
    Coordinate posicionPokemon2 = (Coordinate){.pos_x=7, .pos_y=3};
    TareaPlanificable * tareaCapturaParcial = generarTareaDeCaptura(hiloEntrenadorPlanificable->entrenador, "A", posicionPokemon2);

    hiloEntrenadorPlanificable->ejecutarParcialmente(hiloEntrenadorPlanificable, tareaCapturaParcial, 1);

    assert(tareaCapturaParcial->estado == PENDIENTE_DE_EJECUCION);

    log_info(testLogger, "Testeando el efecto de lado de la tarea de captura parcialmente completada");

    Posicion posicionEnCaminoACapturar = entrenador->posicion(entrenador);
    assert(posicionEnCaminoACapturar.valida == true);
    assert(posicionEnCaminoACapturar.coordenada.pos_x == 4);
    assert(posicionEnCaminoACapturar.coordenada.pos_y == 5);

    tareaCaptura->destruir(tareaCaptura);
    tareaCapturaParcial->destruir(tareaCapturaParcial);
    hiloEntrenadorPlanificable->destruir(hiloEntrenadorPlanificable);
    entrenador->destruir(entrenador);
    mapita.destruir(&mapita);
}
