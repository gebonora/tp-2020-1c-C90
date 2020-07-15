//
// Created by Alan Zhao on 15/07/2020.
//

#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"

static void registrarCapturaExitosa(ServicioDeCaptura * this, CapturaPokemon * capturaPokemon) {
    char * posicion = coordenadaClave(capturaPokemon->pokemonAtrapable->posicionInicial);
    log_info(this->logger, "Se capturó con exito un %s en %s", capturaPokemon->pokemonAtrapable->especie, posicion);

    // TODO: Sacar del mapa al pokemon capturado
    // TODO: Actualizar el estado del entrenador que lo capturó, sumandole 1 a su contabilidad.
    // TODO: Avisarle al servicio de planificacion con NOTIFY_CAUGHT_RESULT que ya puede habilitar al entrenador bloqueado.

    free(posicion);
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
