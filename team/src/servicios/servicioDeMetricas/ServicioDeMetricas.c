/*
 * ServicioDeMetricas.c
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "servicios/servicioDeMetricas/ServicioDeMetricas.h"

static void imprimirMetricas(ServicioDeMetricas* this) {
	log_info(MANDATORY_LOGGER, "Ciclos de CPU ejecutados en total: %d.", this->calcularCiclosTotales(this));
	void imprimirEntrenador(char* entrenadorId, void* cantidad) {
		log_info(MANDATORY_LOGGER, "Ciclos de CPU ejecutados por el entrenador '%s': %d.", entrenadorId, cantidad);
	}
	dictionary_iterator(this->hashMap, imprimirEntrenador);
	log_info(MANDATORY_LOGGER, "Cantidad total de cambios de contexto: %d.", this->cantidadCambiosDeContexto);
	log_info(MANDATORY_LOGGER, "Cantidad total de deadlocks producidos: %d.", this->cantidadDeadlocksProducidos);
	log_info(MANDATORY_LOGGER, "Cantidad total de deadlocks resueltos: %d.", this->cantidadDeadlocksResueltos);
}

static void registrarCicloRealizadoPorEntrenador(ServicioDeMetricas * this, char* entrenadorId) {
	log_debug(this->logger, "Se registró un ciclo para el entrenador: %s.", entrenadorId);
	if (dictionary_has_key(this->hashMap, entrenadorId)) {
		int key = (int) dictionary_get(this->hashMap, entrenadorId);
		key++;
		dictionary_put(this->hashMap, entrenadorId, (int*) key);
	} else
		dictionary_put(this->hashMap, entrenadorId, (int*) 1);
}

static void registrarCambioDeContexto(ServicioDeMetricas* this) {
	log_debug(this->logger, "Se registró un cambio de contexto.");
	(this->cantidadCambiosDeContexto)++;
}

static void registrarDeadlockProducido(ServicioDeMetricas* this) {
	log_debug(this->logger, "Se registró un deadlock producido.");
	(this->cantidadDeadlocksProducidos)++;
}

static void registrarDeadlockResuelto(ServicioDeMetricas* this) {
	log_debug(this->logger, "Se registró un deadlock resuelto.");
	(this->cantidadDeadlocksResueltos)++;
}

int calcularCiclosTotales(ServicioDeMetricas* this) {
	int count = 0;
	void sumarEnCount(char * entrenadorId, void * cantidad) {
		count += (int) cantidad;
	}
	dictionary_iterator(this->hashMap, sumarEnCount);
	return count;
}

static void destruirServicioDeMetricas(ServicioDeMetricas * this) {
	log_debug(this->logger, "Se procede a destruir al servicio de métricas");
	log_destroy(this->logger);
	dictionary_destroy(this->hashMap);
	free(this);
}

static ServicioDeMetricas * new() {
	ServicioDeMetricas * servicio = malloc(sizeof(ServicioDeMetricas));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeMetricas", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_DEBUG);
	servicio->cantidadCambiosDeContexto = 0;
	servicio->cantidadDeadlocksProducidos = 0;
	servicio->cantidadDeadlocksResueltos = 0;
	servicio->hashMap = dictionary_create();
	servicio->imprimirMetricas = &imprimirMetricas;
	servicio->registrarCicloRealizadoPorEntrenador = &registrarCicloRealizadoPorEntrenador;
	servicio->registrarCambioDeContexto = &registrarCambioDeContexto;
	servicio->registrarDeadlockProducido = &registrarDeadlockProducido;
	servicio->registrarDeadlockResuelto = &registrarDeadlockResuelto;
	servicio->calcularCiclosTotales = &calcularCiclosTotales;

	servicio->destruir = &destruirServicioDeMetricas;

	return servicio;
}

const struct ServicioDeMetricasClass ServicioDeMetricasConstructor = { .new = &new };
