/*
 * ServicioDeMetricas.c
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "servicios/servicioDeMetricas/ServicioDeMetricas.h"

static void imprimirMetricas(ServicioDeMetricas* this) {
	log_info(MANDATORY_LOGGER, "----------------------------------------------------------------------------");
	log_info(MANDATORY_LOGGER, "                         Resultado del proceso TEAM                         ");
	log_info(MANDATORY_LOGGER, "----------------------------------------------------------------------------");
	log_info(MANDATORY_LOGGER, "%-70s%6d", "Ciclos de CPU ejecutados en total:", this->calcularCiclosTotales(this));
	void imprimirEntrenador(char* entrenadorId, void* cantidad) {
		char * entrada = string_from_format("Ciclos de CPU ejecutados por el entrenador '%s':", entrenadorId);
		log_info(MANDATORY_LOGGER, "%-70s%6d", entrada, cantidad);
		free(entrada);
	}
	dictionary_iterator(this->hashMap, imprimirEntrenador);
	log_info(MANDATORY_LOGGER, "%-70s%6d", "Cantidad total de cambios de contexto:", this->cantidadCambiosDeContexto);
	log_info(MANDATORY_LOGGER, "%-70s%6d", "Cantidad total de deadlocks producidos:", this->cantidadDeadlocksProducidos);
	log_info(MANDATORY_LOGGER, "%-70s%6d", "Cantidad total de deadlocks resueltos:", this->cantidadDeadlocksResueltos);
	if (list_is_empty(this->deadlocks)) {
        log_info(MANDATORY_LOGGER, "%-70s%6d", "Cantidad de intercambios realizados para resolver los deadlocks: %d", 0);
	}
	for (int a = 0; a < list_size(this->deadlocks); a++) {
		char* entrada = string_from_format("Cantidad de intercambios realizados para resolver el deadlock %d:", a + 1);
		log_info(MANDATORY_LOGGER, "%-70s%6d", entrada, ((RegistroDeadlock*) list_get(this->deadlocks, a))->cantidadIntercambios);
		free(entrada);
	}
	log_info(MANDATORY_LOGGER, "----------------------------------------------------------------------------");
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
	/* No tiene mucho sentido esto a nivel enunciado. Los deadlocks que se produzcan van a ser resueltos, si no se reseulven no se llama a metricas.
	 * podemos directamente marcarlo como resuelto internamente, total esta info se muestra al final.
	 * Capaz para la entrega comentar este logeo.
	 */
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
static void registrarDeadlocks(ServicioDeMetricas* this, t_list* listaDeadlocks) {
	for (int a = 0; a < list_size(listaDeadlocks); a++) {
		RegistroDeadlock* registro = malloc(sizeof(RegistroDeadlock));
		registro->cantidadIntercambios = 0;
		registro->listaDeIdEntrenadores = copiarListaYElementos(((t_list*) list_get(listaDeadlocks, a)));
		list_add(this->deadlocks, registro);
	}
}

static void registrarIntercambio(ServicioDeMetricas* this, char* idDeUnEntrenadorInvolucrado) {
	for (int a = 0; a < list_size(this->deadlocks); a++) {
		RegistroDeadlock* registroActual = (RegistroDeadlock*) list_get(this->deadlocks, a);
		if (perteneceALista(registroActual->listaDeIdEntrenadores, idDeUnEntrenadorInvolucrado)) {
			(registroActual->cantidadIntercambios)++;
			log_debug(this->logger, "Se registró un intercambio para el deadlock %d, entrenador involucrado:'%s'.", a + 1, idDeUnEntrenadorInvolucrado);
			return;
		}
	}
	log_error(this->logger, "El entrenador '%s' no fue registrado como en deadlock. No debería tener un intercambio.", idDeUnEntrenadorInvolucrado);
}

static void destruirServicioDeMetricas(ServicioDeMetricas * this) {
	log_debug(this->logger, "Se procede a destruir al servicio de métricas");
	log_destroy(this->logger);
	dictionary_destroy(this->hashMap);
	void destruirRegistro(void* elem) {
		RegistroDeadlock* registro = (RegistroDeadlock*) elem;
		list_destroy_and_destroy_elements(registro->listaDeIdEntrenadores, free);
		free(registro);
	}
	list_destroy_and_destroy_elements(this->deadlocks, destruirRegistro);
	free(this);
}

static ServicioDeMetricas * new() {
	ServicioDeMetricas * servicio = malloc(sizeof(ServicioDeMetricas));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeMetricas", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_DEBUG);
	servicio->cantidadCambiosDeContexto = 0;
	servicio->cantidadDeadlocksProducidos = 0;
	servicio->cantidadDeadlocksResueltos = 0;
	servicio->hashMap = dictionary_create();
	servicio->deadlocks = list_create();
	servicio->imprimirMetricas = &imprimirMetricas;
	servicio->registrarCicloRealizadoPorEntrenador = &registrarCicloRealizadoPorEntrenador;
	servicio->registrarCambioDeContexto = &registrarCambioDeContexto;
	servicio->registrarDeadlockProducido = &registrarDeadlockProducido;
	servicio->registrarDeadlockResuelto = &registrarDeadlockResuelto;
	servicio->calcularCiclosTotales = &calcularCiclosTotales;
	servicio->registrarDeadlocks = &registrarDeadlocks;
	servicio->registrarIntercambio = &registrarIntercambio;

	servicio->destruir = &destruirServicioDeMetricas;

	return servicio;
}

const struct ServicioDeMetricasClass ServicioDeMetricasConstructor = { .new = &new };
