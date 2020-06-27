//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/equipo/entrenador/Entrenador.h"

bool objetivoCompletado(Entrenador * this) {
    t_list * pokemones = (t_list *) dictionary_keys(this->pokemonesObjetivo);

    int i = 0;
    bool fueCompletado = true;

    while (fueCompletado && i < list_size(pokemones)) {
        char * pokemon = list_get(pokemones, i);
        int cantidadRequerida = (int) dictionary_get(this->pokemonesObjetivo, pokemon);
        int cantidadCapturada = 0;
        if (dictionary_has_key(this->pokemonesCapturados, pokemon)) {
            cantidadCapturada = (int) dictionary_get(this->pokemonesCapturados, pokemon);
        }
        if (cantidadCapturada != cantidadRequerida) {
            fueCompletado = false;
        }
        i++;
    }

    list_destroy(pokemones);

    return fueCompletado;
}

bool puedeAtraparPokemones(Entrenador * this) {
    return totalDePokemones(this->pokemonesCapturados) < this->limiteDeCaptura;
}

Posicion posicion(Entrenador * this) {
    Gps * gps = this->gps;
    if (gps == NULL) {
        log_error(this->logger, "El entrenador no tiene un GPS asignado. No puede responder su ubicación");
        return (Posicion) {.valida = false};
    }
    return gps->posicionActual(gps);
}

static void destruir(Entrenador * this) {
    log_debug(this->logger, "Se procede a destruir un entrenador");
    log_destroy(this->logger);
    dictionary_destroy(this->pokemonesCapturados);
    dictionary_destroy(this->pokemonesObjetivo);
    if (this->gps) {
        this->gps->destruirGps(this->gps);
    }
    free(this);
}

static Entrenador *new(char * posicionInicial, char * pokemonesIniciales, char * pokemonesObjetivos) {
    Entrenador * entrenador = malloc(sizeof(Entrenador));

    entrenador->logger = log_create(TEAM_INTERNAL_LOG_FILE, "Entrenador", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
    entrenador->gps = NULL;
    entrenador->posicionInicial = parsearPosicion(posicionInicial, "|");
    entrenador->tipoPosicionable = ENTRENADOR;
    entrenador->pokemonesCapturados = agruparPokemonesPorNombre(pokemonesIniciales);
    entrenador->pokemonesObjetivo = agruparPokemonesPorNombre(pokemonesObjetivos);
    entrenador->limiteDeCaptura = totalDePokemones(entrenador->pokemonesObjetivo);
    entrenador->objetivoCompletado = &objetivoCompletado;
    entrenador->puedeAtraparPokemones = &puedeAtraparPokemones;
    entrenador->posicion = &posicion;
    entrenador->destruir = &destruir;

    log_debug(entrenador->logger, "Se instanció un entrenador con pokemones iniciales: %s, y objetivos: %s", pokemonesIniciales, pokemonesObjetivos);

    return entrenador;
}

const struct EntrenadorClass EntrenadorConstructor = {.new=&new};

// Funciones estaticas
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