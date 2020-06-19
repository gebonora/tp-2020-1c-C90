//
// Created by Alan Zhao on 17/06/2020.
//

#include "modelo/equipo/Equipo.h"


Equipo crearEquipoPorConfiguracion() {
    Equipo equipo = list_create();

    t_list * posicionDeLosEntrenadores = servicioDeConfiguracion
            .obtenerLista(&servicioDeConfiguracion, POSICIONES_ENTRENADORES);

    for (int i=0; i < list_size(posicionDeLosEntrenadores); i++) {
        char * posicionInicial = list_get(posicionDeLosEntrenadores, i);
        Entrenador * entrenador = EntrenadorConstructor.new(posicionInicial, "", "");
        list_add(equipo, entrenador);
    }

    list_destroy_and_destroy_elements(posicionDeLosEntrenadores, free);

    return equipo;
}

void destruirEntrenador(Entrenador * entrenador) {
    entrenador->destruir(entrenador);
}

void destruirEquipo(Equipo equipo) {
    list_destroy_and_destroy_elements(equipo, (void (*)(void *)) destruirEntrenador);
}