//
// Created by Alan Zhao on 13/06/2020.
//

#include "modelo/equipo/entrenador/Entrenador.h"

static bool reemplazarPokemonCapturadoPor(Entrenador * this, char * especiePreviamenteCapturada, char * especieNueva) {
    if (this->tieneEspecieCapturada(this, especiePreviamenteCapturada)) {
        this->quitarPokemon(this, especiePreviamenteCapturada);
        this->registrarCaptura(this, especieNueva);
        return true;
    } else {
        log_error(this->logger, "Se solicitó reemplazar un pokemon que no existe en el entrenador");
        return false;
    }
}

static void quitarPokemon(Entrenador * this, char * especie) {
    if (this->tieneEspecieCapturada(this, especie)) {
        int * ejemplares = dictionary_get(this->pokemonesCapturados, especie);
        (*ejemplares)--;
    }
}

static bool tieneEspecieCapturada(Entrenador * this, char * especie) {
    return dictionary_has_key(this->pokemonesCapturados, especie) && *(int *) dictionary_get(this->pokemonesCapturados, especie) > 0;
}

static bool intercambiarPokemon(Entrenador * this, Entrenador * entrenadorInteresado, char * pokemonAEntregar, char * pokemonARecibir) {
    log_info(this->logger, "Iniciando el intercambio de un %s propio por un %s ajeno con %s", pokemonAEntregar, pokemonARecibir, entrenadorInteresado->id);
    bool seCumplenLosRequisitosParaElIntercambio = true;

    if (!this->tieneEspecieCapturada(this, pokemonAEntregar)) {
        log_error(this->logger, "No se pudo realizar el intercambio porque no se dispone de un %s para entregar", pokemonAEntregar);
        seCumplenLosRequisitosParaElIntercambio = false;
    } else if (!entrenadorInteresado->tieneEspecieCapturada(entrenadorInteresado, pokemonARecibir)) {
        log_error(this->logger, "No se pudo realizar el intercambio porque %s no tiene un %s", entrenadorInteresado->id, pokemonARecibir);
        seCumplenLosRequisitosParaElIntercambio = false;
    }

    if (seCumplenLosRequisitosParaElIntercambio) {
        log_debug(this->logger, "Se procede a eliminar a %s del inventario y agregarlo a %s", pokemonAEntregar, entrenadorInteresado->id);
        this->reemplazarPokemonCapturadoPor(this, pokemonAEntregar, pokemonARecibir);
        log_debug(this->logger, "Se procede a eliminar a %s del inventario y agregarlo a %s", pokemonAEntregar, entrenadorInteresado->id);
        entrenadorInteresado->reemplazarPokemonCapturadoPor(entrenadorInteresado, pokemonARecibir, pokemonAEntregar);
        log_info(MANDATORY_LOGGER, "Intercambio exitoso. %s le dio un %s a %s y a cambio recibió un %s. ", this->id, pokemonAEntregar, entrenadorInteresado->id, pokemonARecibir);
    }
    return seCumplenLosRequisitosParaElIntercambio;
}

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
        log_info(MANDATORY_LOGGER,"El entrenador %s se movió de %s a %s", this->id, coordActualImprimible, coordObjetivoImprimible);
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
            log_debug(this->logger, "No se cumplio el objetivo porque se tiene %d/%d %s", cantidadCapturada, cantidadRequerida, pokemon);
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
    entrenador->intercambiarPokemon = &intercambiarPokemon;
    entrenador->tieneEspecieCapturada = &tieneEspecieCapturada;
    entrenador->quitarPokemon = &quitarPokemon;
    entrenador->reemplazarPokemonCapturadoPor = &reemplazarPokemonCapturadoPor;
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
