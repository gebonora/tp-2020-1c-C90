//
// Created by Alan Zhao on 15/07/2020.
//

#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"

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
    servicio->destruir = &destruirServicioDeCaptura;

    return servicio;
}

const struct ServicioDeCapturaClass ServicioDeCapturaConstructor = {.new=&new};
