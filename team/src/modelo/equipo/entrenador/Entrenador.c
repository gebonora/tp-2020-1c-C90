//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/equipo/entrenador/Entrenador.h"

static void destruir(Entrenador * this) {
    log_destroy(this->logger);
    dictionary_destroy(this->pokemones_iniciales);
    dictionary_destroy(this->pokemones_objetivo);
    free(this);
}

Coordinate parsearPosicion(char * posicion) {
    char ** punto = string_n_split(posicion, 2, "|");
    Coordinate coordenada = {.pos_x=atoi(punto[0]), .pos_y=atoi(punto[1])};

    free(punto[0]);
    free(punto[1]);
    free(punto);

    return coordenada;
}

ContadorPokemones nombreDeLosPokemones(char * nombresDeLosPokemones) {
    return dictionary_create();
}

static Entrenador *new(char * posicionInicial, char * pokemones_iniciales, char * pokemones_objetivos) {
    Entrenador * entrenador = malloc(sizeof(Entrenador));

    entrenador->logger = log_create(TEAM_INTERNAL_LOG_FILE, "Entrenador", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    entrenador->posicionInicial = parsearPosicion(posicionInicial);
    entrenador->pokemones_iniciales = nombreDeLosPokemones(pokemones_iniciales);
    entrenador->pokemones_objetivo = nombreDeLosPokemones(pokemones_objetivos);
    entrenador->destruir = &destruir;

    return entrenador;
}

const struct EntrenadorClass EntrenadorConstructor = {.new=&new};