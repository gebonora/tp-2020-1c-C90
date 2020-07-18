/*
 * ServicioDeResolucionDeDeadlocks.c
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "servicios/servicioDeResolucionDeDeadlocks/ServicioDeResolucionDeDeadlocks.h"

bool detectarDeadlock(ServicioDeResolucionDeDeadlocks * this, t_list* entrenadoresBloqueados) {
	/* Me llegan los entrenadores de la cola de Blocked.
	 * 		Tengo que quedarme con los que están llenos y no completaron su objetivo y luego analizar si están en deadlock.
	 */

	bool posibleDeadlock(void* elem) {
		Entrenador* entrenador = (Entrenador*) elem;
		return !entrenador->puedeAtraparPokemones(entrenador) && !entrenador->objetivoCompletado(entrenador);
	}
	t_list* entrenadoresFiltrados = list_filter(entrenadoresBloqueados, posibleDeadlock); // TODO: Esta copia hay que liberarla?

	return true;
}

static void destruirServicioDeResolucionDeDeadlocks(ServicioDeResolucionDeDeadlocks * this) {
	log_debug(this->logger, "Se procede a destruir al servicio de resolución de deadlocks");
	log_destroy(this->logger);
	free(this);
}

static ServicioDeResolucionDeDeadlocks * new(Mapa mapa, Equipo equipo, ServicioDePlanificacion * servicioDePlanificacion) {
	ServicioDeResolucionDeDeadlocks * servicio = malloc(sizeof(ServicioDeResolucionDeDeadlocks));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeResolucionDeDeadlocks", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_DEBUG);
	servicio->detectarDeadlock = &detectarDeadlock;
	servicio->destruir = &destruirServicioDeResolucionDeDeadlocks;

	return servicio;
}

const struct ServicioDeResolucionDeDeadlocksClass ServicioDeResolucionDeDeadlocksConstructor = { .new = &new };
