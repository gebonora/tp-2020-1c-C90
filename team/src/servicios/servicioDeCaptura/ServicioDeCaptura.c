//
// Created by Alan Zhao on 15/07/2020.
//

#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"

#define mismoIdEntrenador(idEntrenador) \
bool matcheaIdEntrenador(void * entrenador) { \
    return string_equals(((Entrenador *) entrenador)->id, idEntrenador); \
}

static bool registrarCapturaExitosa(ServicioDeCaptura * this, CapturaPokemon * capturaPokemon) {
    char * posicion = capturaPokemon->posicion(capturaPokemon);
    bool sePudoRegistrar = false;

    mismoIdEntrenador(capturaPokemon->idEntrenador);
    Entrenador * entrenador = list_find(this->equipo, matcheaIdEntrenador);
    if (entrenador != NULL) {
        log_debug(this->logger, "Existe el entrenador que capturó al pokemon. Se procede con los efectos de lado.");
        log_debug(this->logger, "Eliminando al pokemon capturado del mapa...");
        bool sePudoEliminarDelMapa = capturaPokemon->eliminarPokemonCapturadoDelMapa(capturaPokemon, this->mapa);
        if (sePudoEliminarDelMapa) {
            // TODO: Actualizar el estado del entrenador que lo capturó, sumandole 1 a su contabilidad.
            // TODO: Avisarle al servicio de planificacion con NOTIFY_CAUGHT_RESULT que ya puede habilitar al entrenador bloqueado.
        }
        sePudoRegistrar = true;
        log_info(this->logger, "%s capturó con exito un %s en %s", capturaPokemon->idEntrenador, capturaPokemon->pokemonAtrapable->especie, posicion);
    } else {
        log_error(this->logger, "No existe un entrenador %s en el equipo que haya intentado capturar al pokemon", capturaPokemon->idEntrenador);
    }
    free(posicion);
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
    servicio->destruir = &destruirServicioDeCaptura;

    return servicio;
}

const struct ServicioDeCapturaClass ServicioDeCapturaConstructor = {.new=&new};