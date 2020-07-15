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
            t_list * args = list_create();
            list_add(args, create_coordinate(punteroCoordenada->pos_x, punteroCoordenada->pos_y));

            Instruccion * instruccion = malloc(sizeof(Instruccion));

            instruccion->posicion = i;
            instruccion->funcion = (void (*)(void *, ...)) entrenador->mover;
            instruccion->descripcion = string_from_format("Mover entrenador a (%d,%d)", punteroCoordenada->pos_x, punteroCoordenada->pos_y);
            instruccion->argumentos = args;

            list_add(instrucciones, instruccion);
        }

        TareaPlanificable * tareaDeCaptura = TareaPlanificableConstructor.new(instrucciones);
        list_destroy_and_destroy_elements(camino, free);

        return tareaDeCaptura;
    }

    log_error(INTERNAL_LOGGER, "No es posible generar una tarea de captura para %s ya que no tiene una posicion valida.", entrenador->id);

    return NULL;
}