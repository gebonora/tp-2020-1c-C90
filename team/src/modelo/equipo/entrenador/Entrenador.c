//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/equipo/entrenador/Entrenador.h"

static void registrarCaptura(Entrenador * this, char * especie){
    log_info(this->logger, "Se agrega un %s a los pokemons capturados", especie);
    if (dictionary_has_key(this->pokemonesCapturados, especie)) {
        int * ejemplares = dictionary_get(this->pokemonesCapturados, especie);
        (*ejemplares)++;
    } else {
        int * nuevoContador = malloc(sizeof(int));
        *nuevoContador = 1;
        dictionary_put(this->pokemonesCapturados, especie, nuevoContador);
    }
}

void mover(Entrenador * this, Coordinate * posicionObjetivo) {
    Posicion posicionActual = this->posicion(this);
    if (posicionActual.valida) {

        char * coordActualImprimible = coordenadaClave(posicionActual.coordenada);
        char * coordObjetivoImprimible = coordenadaPuntero(posicionObjetivo);

        int distanciaARecorrer = distanciaEntre(posicionActual.coordenada, convertirACoordenada(posicionObjetivo));
        if (distanciaARecorrer != 1) {
            log_warning(this->logger, "Se esta intentando mover a un entrenador de forma no adyacente [%s -> %s]. Distancia: %d.",
                    coordActualImprimible, coordObjetivoImprimible, distanciaARecorrer);
        }

        Gps * gps = this->gps;
        gps->irA(gps, convertirACoordenada(posicionObjetivo));
        log_debug(this->logger,"El entrenador se movió de %s a %s", coordActualImprimible, coordObjetivoImprimible);
        free(coordActualImprimible);
        free(coordObjetivoImprimible);
    } else {
        log_error(this->logger, "No se puede mover a un entrenador sin posicion actual");
    }
}

bool objetivoCompletado(Entrenador * this) {
    t_list * pokemones = (t_list *) dictionary_keys(this->pokemonesObjetivo);

    int i = 0;
    bool fueCompletado = true;

    while (fueCompletado && i < list_size(pokemones)) {
        char * pokemon = list_get(pokemones, i);
        int cantidadRequerida = *(int *) dictionary_get(this->pokemonesObjetivo, pokemon);
        int cantidadCapturada = 0;
        if (dictionary_has_key(this->pokemonesCapturados, pokemon)) {
            cantidadCapturada = *(int *) dictionary_get(this->pokemonesCapturados, pokemon);
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
        log_error(this->logger, "El entrenador no tiene un GPS asignado. No puede responder su ubicación.");
        return (Posicion) {.valida = false};
    }
    return gps->posicionActual(gps);
}

static char * descripcion(Entrenador * this) {
    return this->id;
}

static void destruir(Entrenador * this) {
    log_debug(this->logger, "Se procede a destruir al entrenador");
    log_destroy(this->logger);
    dictionary_destroy_and_destroy_elements(this->pokemonesCapturados, free);
    dictionary_destroy_and_destroy_elements(this->pokemonesObjetivo, free);
    if (this->gps) {
        this->gps->destruirGps(this->gps);
    }
    free(this->id);
    free(this);
}

static Entrenador *new(char * posicionInicial, char * pokemonesIniciales, char * pokemonesObjetivos) {
    Entrenador * entrenador = malloc(sizeof(Entrenador));

    entrenador->id = string_from_format("Entrenador_%d", ++contadorEntrenadores);
    entrenador->logger = log_create(TEAM_INTERNAL_LOG_FILE, entrenador->id, SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
    entrenador->gps = NULL;
    entrenador->posicionInicial = parsearPosicion(posicionInicial, "|");
    entrenador->tipoPosicionable = ENTRENADOR;
    entrenador->pokemonesCapturados = agruparPokemonesPorNombre(pokemonesIniciales);
    entrenador->pokemonesObjetivo = agruparPokemonesPorNombre(pokemonesObjetivos);
    entrenador->estaEsperandoAlgo = false;
    entrenador->limiteDeCaptura = totalDePokemones(entrenador->pokemonesObjetivo);
    entrenador->mover = &mover;
    entrenador->objetivoCompletado = &objetivoCompletado;
    entrenador->puedeAtraparPokemones = &puedeAtraparPokemones;
    entrenador->posicion = &posicion;
    entrenador->descripcion = &descripcion;
    entrenador->registrarCaptura = &registrarCaptura;
    entrenador->destruir = &destruir;

    log_debug(entrenador->logger, "Se instanció al entrenador con pokemones iniciales: %s, y objetivos: %s", pokemonesIniciales, pokemonesObjetivos);

    return entrenador;
}

const struct EntrenadorClass EntrenadorConstructor = {.new=&new};

// Funciones estaticas
ContadorPokemones agruparPokemonesPorNombre(char * nombresDeLosPokemones) {
    ContadorPokemones contador = dictionary_create();

    if (!string_equals_ignore_case(nombresDeLosPokemones, NO_POKEMON)) {
        char ** arrayPokemon = string_split(nombresDeLosPokemones, "|");

        t_list * pokemones = (t_list *) list_from((void **) arrayPokemon);

        for (int i=0; i < list_size(pokemones); i++) {
            char * nombrePokemon = list_get(pokemones, i);
            if (dictionary_has_key(contador, nombrePokemon)) {
                int * ejemplares = dictionary_get(contador, nombrePokemon);
                (*ejemplares)++;
            } else {
                int * nuevoContador = malloc(sizeof(int));
                *nuevoContador = 1;
                dictionary_put(contador, nombrePokemon, nuevoContador);
            }
        }

        list_destroy_and_destroy_elements(pokemones, free);
        free(arrayPokemon);
    }

    return contador;
}

int totalDePokemones(ContadorPokemones contadorPokemones) {
    int total = 0;
    void sumar_al_total(char * pokemon, void * cantidad){
        total += *(int *)cantidad;
    }
    dictionary_iterator(contadorPokemones, sumar_al_total);
    return total;
}
