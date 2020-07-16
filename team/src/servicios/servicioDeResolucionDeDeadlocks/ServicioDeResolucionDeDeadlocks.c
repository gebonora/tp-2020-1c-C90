/*
 * ServicioDeResolucionDeDeadlocks.c
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "servicios/servicioDeResolucionDeDeadlocks/ServicioDeResolucionDeDeadlocks.h"

void destruirServicioDeResolucionDeDeadlocks(ServicioDeResolucionDeDeadlocks * this) {
    log_debug(this->logger, "Se procede a destruir al servicio de captura");
    log_destroy(this->logger);
    free(this);
}

static ServicioDeResolucionDeDeadlocks * new(Mapa mapa, Equipo equipo, ServicioDePlanificacion * servicioDePlanificacion) {
	ServicioDeResolucionDeDeadlocks * servicio = malloc(sizeof(ServicioDeResolucionDeDeadlocks));

    servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeCaptura", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    servicio->mapa = mapa;
    servicio->equipo = equipo;
    servicio->servicioDePlanificacion = servicioDePlanificacion;
    servicio->destruir = &destruirServicioDeResolucionDeDeadlocks;

    return servicio;
}

const struct ServicioDeResolucionDeDeadlocksClass ServicioDeResolucionDeDeadlocksConstructor = {.new=&new};
