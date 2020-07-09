//
// Created by Alan Zhao on 07/07/2020.
//

#include "modelo/planificable/tarea/captura/TareaDeCaptura.h"

TareaPlanificable * generarTareaDeCaptura(Entrenador * entrenador, char * pokemon, Coordinate posicionPokemon) {
    Posicion posicionOrigen = entrenador->posicion(entrenador);
    if (posicionOrigen.valida){
        Camino camino = generarCamino(posicionOrigen.coordenada, posicionPokemon);
        Instrucciones instrucciones = list_create();
        for (int i = 0; i < list_size(camino); i++) {
            Coordinate * punteroCoordenada = list_get(camino, i);
            Coordinate coordenada = {.pos_x=0, .pos_y=0};
            Instruccion * instruccion = malloc(sizeof(Instruccion));
            instruccion->posicion = i;
            instruccion->funcion = entrenador->mover;
            char * coordenadaImprimible = coordenadaClave(coordenada);
            instruccion->descripcion = string_from_format("Moviendo entrenador a %s", coordenadaImprimible);
            free(coordenadaImprimible);
        }
        TareaPlanificable * tareaDeCaptura = TareaPlanificableConstructor.new(instrucciones);
        list_destroy(camino);
        return tareaDeCaptura;
    }
    log_error(INTERNAL_LOGGER, "No es posible generar una tarea de captura para %s ya que no tiene una posicion valida.", entrenador->id);
    return NULL;
}

void moverA(Entrenador * entrenador, Coordinate posicionObjetivo) {

}