/*
 * ServicioDeMetricas.c
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "servicios/servicioDeMetricas/ServicioDeMetricas.h"

void destruirServicioDeMetricas(ServicioDeMetricas * this) {
	log_debug(this->logger, "Se procede a destruir al servicio de métricas");
	log_destroy(this->logger);
	free(this);
}

static ServicioDeMetricas * new() {
	ServicioDeMetricas * servicio = malloc(sizeof(ServicioDeMetricas));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeMetricas", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
	servicio->destruir = &destruirServicioDeMetricas;

	return servicio;
}

const struct ServicioDeMetricasClass ServicioDeMetricasConstructor = { .new = &new };
