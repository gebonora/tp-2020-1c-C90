//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/equipo/entrenador/Entrenador.h"

static void destruir(Entrenador * this) {
    log_destroy(this->logger);
    dictionary_destroy(this->pokemonesIniciales);
    dictionary_destroy(this->pokemonesObjetivo);
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

ContadorPokemones agruparPokemonesPorNombre(char * nombresDeLosPokemones) {
    ContadorPokemones contador = dictionary_create();

    char ** arrayPokemon = string_split(nombresDeLosPokemones, "|");

    t_list * pokemones = (t_list *) list_from((void **) arrayPokemon);

    for (int i=0; i < list_size(pokemones); i++) {
        char * nombrePokemon = list_get(pokemones, i);
        if (dictionary_has_key(contador, nombrePokemon)) {
            dictionary_put(contador, nombrePokemon, dictionary_get(contador, nombrePokemon) + 1);
        } else {
            dictionary_put(contador, nombrePokemon, (void *) 1);
        }
    }

    list_destroy_and_destroy_elements(pokemones, free);
    free(arrayPokemon);

    return contador;
}

int totalDePokemones(ContadorPokemones contadorPokemones) {
    int total = 0;
    void sumar_al_total(char * pokemon, void * cantidad){
        total += (int) cantidad;
    }
    dictionary_iterator(contadorPokemones, sumar_al_total);
    return total;
}

static Entrenador *new(char * posicionInicial, char * pokemones_iniciales, char * pokemones_objetivos) {
    Entrenador * entrenador = malloc(sizeof(Entrenador));

    ContadorPokemones pokemonesObjetivo = agruparPokemonesPorNombre(pokemones_objetivos);

    entrenador->logger = log_create(TEAM_INTERNAL_LOG_FILE, "Entrenador", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    entrenador->posicionInicial = parsearPosicion(posicionInicial);
    entrenador->pokemonesIniciales = agruparPokemonesPorNombre(pokemones_iniciales);
    entrenador->pokemonesObjetivo = pokemonesObjetivo;
    entrenador->limiteDeCaptura = totalDePokemones(entrenador->pokemonesObjetivo);
    entrenador->destruir = &destruir;

    return entrenador;
}

const struct EntrenadorClass EntrenadorConstructor = {.new=&new};