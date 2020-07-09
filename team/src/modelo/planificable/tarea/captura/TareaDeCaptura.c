//
// Created by Alan Zhao on 07/07/2020.
//

#include "modelo/planificable/tarea/captura/TareaDeCaptura.h"

TareaPlanificable * generarTareaDeCaptura(Entrenador * entrenador, char * pokemon, Coordinate posicionPokemon) {
    Posicion posicionOrigen = entrenador->posicion(entrenador);
    if (posicionOrigen.valida){
        Camino camino = generarCamino(posicionOrigen.coordenada, posicionPokemon);
        TareaPlanificable * tareaDeCaptura = TareaPlanificableConstructor.new(list_create());
        list_destroy(camino);
        return tareaDeCaptura;
    }
    log_error(INTERNAL_LOGGER, "No es posible generar una tarea de captura para %s ya que no tiene una posicion valida.", entrenador->id);
    return NULL;
}