//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/equipo/entrenador/Entrenador.h"

static void destruir(Entrenador * this) {
    log_destroy(this->logger);
    free(this);
}

Coordinate parsearPosicion(char * posicion) {
    char ** punto = string_split(posicion, "|");
    Coordinate coordenada = {.pos_x=atoi(punto[0]), .pos_y=atoi(punto[1])};
    return coordenada;
}

static Entrenador *new(char * posicionInicial) {
    Entrenador * entrenador = malloc(sizeof(Entrenador));

    entrenador->logger = log_create(TEAM_INTERNAL_LOG_FILE, "Entrenador", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    entrenador->posicionInicial = parsearPosicion(posicionInicial);
    entrenador->destruir = &destruir;

    return entrenador;
}

const struct EntrenadorClass EntrenadorConstructor = {.new=&new};