//
// Created by Alan Zhao on 11/07/2020.
//

#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"

void trabajar(ServicioDePlanificacion * this) {
    log_info(this->logger, "Trabajo muy duro siendo un servicio");
}

void destruir(ServicioDePlanificacion * this) {
    log_debug(this->logger, "Se procede a destruir al servicio de planificacion");
    queue_destroy(this->colaDeTrabajo);
    log_destroy(this->logger);
}

static ServicioDePlanificacion new() {
    ServicioDePlanificacion servicio = {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDePlanificacion", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO),
            .colaDeTrabajo = queue_create(),
            &trabajar,
            &destruir
    };
    //crearHilo(servicio.trabajar, servicio);
    return servicio;
}

const struct ServicioDePlanificacionClass ServicioDePlanificacionConstructor = {.new=&new};