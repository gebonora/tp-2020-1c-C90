//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeTareas() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestTareas", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando la generacion de una tarea de captura");
    Entrenador * entrenador = EntrenadorConstructor.new("4|1", "A", "B");
    Mapa mapita = MapaConstructor.new();
    registrarEnMapaPosicionEntrenador(&mapita, entrenador);
    Coordinate posicionPokemon = (Coordinate){.pos_x=6, .pos_y=1};
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
    log_destroy(testLogger);
}