//
// Created by Alan Zhao on 24/07/2020.
//

#include "modelo/planificable/tarea/intercambio/TareaDeIntercambio.h"

TareaPlanificable * generarTareaDeIntercambio(
        Entrenador * entrenadorMovil,
        Entrenador * entrenadorEstatico,
        char * pokemonAEntregar,
        char * pokemonARecibir
        ) {
    Posicion posicionOrigen = entrenadorMovil->posicion(entrenadorMovil);
    Posicion posicionDestino = entrenadorEstatico->posicion(entrenadorEstatico);

    if (posicionOrigen.valida){
        Camino camino = generarCamino(posicionOrigen.coordenada, posicionDestino.coordenada);
        Instrucciones instrucciones = list_create();

        for (int i = 0; i < list_size(camino); i++) {
            Coordinate * punteroCoordenada = list_get(camino, i);
            t_list * args = list_create();
            list_add(args, create_coordinate(punteroCoordenada->pos_x, punteroCoordenada->pos_y));

            Instruccion * instruccion = malloc(sizeof(Instruccion));

            instruccion->posicion = i;
            instruccion->funcion = (void (*)(void *, ...)) entrenadorMovil->mover;
            instruccion->descripcion = string_from_format("Mover entrenador a (%d,%d)", punteroCoordenada->pos_x, punteroCoordenada->pos_y);
            instruccion->argumentos = args;

            list_add(instrucciones, instruccion);
        }

        t_list * argumentosCaptura = list_create();
        list_add(argumentosCaptura, string_duplicate(entrenadorEstatico->id));
        list_add(argumentosCaptura, string_duplicate(pokemonAEntregar));
        list_add(argumentosCaptura, string_duplicate(pokemonARecibir));

        Instruccion * instruccionFinal = crearInstruccion(
                list_size(instrucciones),
                (void (*)(void *, ...)) intercambiarPokemones,
                string_from_format("Intercambio - %s entrega un %s y recibe a cambio un %s de %s ", entrenadorMovil->id),
                argumentosCaptura
        );
        list_add(instrucciones, instruccionFinal);

        TareaPlanificable * tareaDeCaptura = TareaPlanificableConstructor.new(instrucciones);
        list_destroy_and_destroy_elements(camino, free);

        return tareaDeCaptura;
    }

    log_error(INTERNAL_LOGGER, "No es posible generar una tarea de intercambio para %s ya que no tiene una posicion valida.", entrenadorMovil->id);

    return NULL;
}

void intercambiarPokemones(Entrenador * entrenadorMovil, char * idEntrenadorEstatico, char * pokemonAEntrengar, char * pokemonARecibir) {
    log_info(entrenadorMovil->logger, "Intercambio mi %s por un %s con %s", pokemonAEntrengar, pokemonARecibir, idEntrenadorEstatico);
}