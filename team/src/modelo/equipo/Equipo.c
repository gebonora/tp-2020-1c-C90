//
// Created by Alan Zhao on 17/06/2020.
//

#include "modelo/equipo/Equipo.h"


Equipo crearEquipoPorConfiguracion() {
    Equipo equipo = list_create();

    t_list * posicionDeLosEntrenadores = servicioDeConfiguracion
            .obtenerLista(&servicioDeConfiguracion, POSICIONES_ENTRENADORES);

    t_list * pokemonesInicialesDeLosEntrenadores = servicioDeConfiguracion
            .obtenerLista(&servicioDeConfiguracion, POKEMON_ENTRENADORES);

    t_list * pokemonesObjetivoDeLosEntrenadores = servicioDeConfiguracion
            .obtenerLista(&servicioDeConfiguracion, OBJETIVOS_ENTRENADORES);

    for (int i=0; i < list_size(posicionDeLosEntrenadores); i++) {
        char * posicionInicial = list_get(posicionDeLosEntrenadores, i);
        char * pokemonesIniciales = list_get(pokemonesInicialesDeLosEntrenadores, i);
        char * pokemonesObjetivo = list_get(pokemonesObjetivoDeLosEntrenadores, i);
        Entrenador * entrenador = EntrenadorConstructor.new(posicionInicial, pokemonesIniciales, pokemonesObjetivo);
        list_add(equipo, entrenador);
    }

    list_destroy_and_destroy_elements(posicionDeLosEntrenadores, free);
    list_destroy_and_destroy_elements(pokemonesInicialesDeLosEntrenadores, free);
    list_destroy_and_destroy_elements(pokemonesObjetivoDeLosEntrenadores, free);

    return equipo;
}

void destruirEntrenador(Entrenador * entrenador) {
    entrenador->destruir(entrenador);
}

void destruirEquipo(Equipo equipo) {
    list_destroy_and_destroy_elements(equipo, (void (*)(void *)) destruirEntrenador);
}