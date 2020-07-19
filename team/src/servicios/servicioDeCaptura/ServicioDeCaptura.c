//
// Created by Alan Zhao on 15/07/2020.
//

#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"

#define mismoIdEntrenador(idEntrenador) \
bool matcheaIdEntrenador(void * entrenador) { \
    return string_equals(((Entrenador *) entrenador)->id, idEntrenador); \
}

static void procesarPokemonCapturable(ServicioDeCaptura * this, char * especie, Coordinate posicion) {
    this->altaDePokemon(this, especie, posicion);
    bool sePuedeCapturar = false;
    if (sePuedeCapturar) {
        log_debug(this->logger, "Se determinó que es posible la captura de %s", especie);
        this->encargarTrabajoDeCaptura(this, especie, posicion);
    } else {
        log_warning(this->logger, "No hay nadie disponible que pueda capturar a %s", especie);
    }
}

static void altaDePokemon(ServicioDeCaptura * this, char * especie, Coordinate posicion) {
    char * ubicacionPokemonACapturar = separarCoordenada(posicion, "|");
    PokemonAtrapable * pokemonAtrapable = PokemonAtrapableConstructor.new(especie, ubicacionPokemonACapturar);
    registrarEnMapaPosicionPokemonAtrapable(&this->mapa, pokemonAtrapable);
    log_info(this->logger, "Agregamos un %s en la posición %s", especie, ubicacionPokemonACapturar);
    pokemonAtrapable->destruir(pokemonAtrapable);
    free(ubicacionPokemonACapturar);
}

static void encargarTrabajoDeCaptura(ServicioDeCaptura * this, char * especie, Coordinate posicion) {
    char * ubicacionPokemonACapturar = coordenadaClave(posicion);
    log_info(this->logger, "Se le encarga al servicio de planificacion que mande a un entrenador a capturar a %s en %s", especie, ubicacionPokemonACapturar);
    free(ubicacionPokemonACapturar);
}

static bool registrarCapturaExitosa(ServicioDeCaptura * this, CapturaPokemon * capturaPokemon) {
    bool sePudoRegistrar = false;
    log_debug(this->logger, "Intentando impactar en el sistema la captura de %s por parte de %s", capturaPokemon->especie(capturaPokemon), capturaPokemon->idEntrenador);
    mismoIdEntrenador(capturaPokemon->idEntrenador);
    Entrenador * entrenador = list_find(this->equipo, matcheaIdEntrenador);
    if (entrenador != NULL) {
        log_debug(this->logger, "Existe el entrenador que capturó al pokemon. Se procede con los efectos de lado.");
        log_debug(this->logger, "Eliminando al pokemon capturado del mapa...");
        bool sePudoEliminarDelMapa = capturaPokemon->eliminarPokemonCapturadoDelMapa(capturaPokemon, this->mapa);
        if (sePudoEliminarDelMapa) {
            entrenador->registrarCaptura(entrenador, capturaPokemon);
            // TODO: Avisarle al servicio de planificacion con NOTIFY_CAUGHT_RESULT que ya puede habilitar al entrenador bloqueado.
        }
        sePudoRegistrar = true;
        char * posicion = capturaPokemon->posicion(capturaPokemon);
        log_info(this->logger, "%s capturó con exito un %s en %s", capturaPokemon->idEntrenador, capturaPokemon->pokemonAtrapable->especie, posicion);
        free(posicion);
    } else {
        log_error(this->logger, "No existe un entrenador %s en el equipo que haya intentado capturar al pokemon", capturaPokemon->idEntrenador);
    }
    return sePudoRegistrar;
}

void destruirServicioDeCaptura(ServicioDeCaptura * this) {
    log_debug(this->logger, "Se procede a destruir al servicio de captura");
    log_destroy(this->logger);
    free(this);
}

static ServicioDeCaptura * new(Mapa mapa, Equipo equipo, ServicioDePlanificacion * servicioDePlanificacion) {
    ServicioDeCaptura * servicio = malloc(sizeof(ServicioDeCaptura));

    servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeCaptura", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    servicio->mapa = mapa;
    servicio->equipo = equipo;
    servicio->servicioDePlanificacion = servicioDePlanificacion;
    servicio->registrarCapturaExitosa = &registrarCapturaExitosa;
    servicio->procesarPokemonCapturable = &procesarPokemonCapturable;
    servicio->encargarTrabajoDeCaptura = &encargarTrabajoDeCaptura;
    servicio->altaDePokemon = &altaDePokemon;
    servicio->destruir = &destruirServicioDeCaptura;

    return servicio;
}

const struct ServicioDeCapturaClass ServicioDeCapturaConstructor = {.new=&new};
