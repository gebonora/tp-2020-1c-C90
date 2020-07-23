/*
 * ServicioDeMetricas.h
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#ifndef INCLUDE_SERVICIOS_SERVICIODEMETRICAS_SERVICIODEMETRICAS_H_
#define INCLUDE_SERVICIOS_SERVICIODEMETRICAS_SERVICIODEMETRICAS_H_

#include "app/Global.h"
#include <delibird/utils/colecciones/ExtensionColecciones.h>

/*
 * Esta clase se encarga de registrar metricas e imprimirlas.
 *
 */

typedef struct RegistroDeadlock {
	t_list* listaDeIdEntrenadores;
	int cantidadIntercambios;
} RegistroDeadlock;

typedef t_list* listaRegistrosDeadlock;

typedef struct ServicioDeMetricas {
	t_log * logger;
	int cantidadCambiosDeContexto;
	int cantidadDeadlocksProducidos;
	int cantidadDeadlocksResueltos;
	t_dictionary* hashMap;
	listaRegistrosDeadlock deadlocks;
	//Interfaz publica
	void (*imprimirMetricas)(struct ServicioDeMetricas* this);
	void (*registrarCicloRealizadoPorEntrenador)(struct ServicioDeMetricas* this, char* entrenadorId);
	void (*registrarCambioDeContexto)(struct ServicioDeMetricas* this);
	void (*registrarDeadlockProducido)(struct ServicioDeMetricas* this);
	void (*registrarDeadlockResuelto)(struct ServicioDeMetricas* this);
	void (*registrarDeadlocks)(struct ServicioDeMetricas* this, t_list* listaDeadlocks); // Acoplada al formato de lista ServicioDeResolucionDeadlocks
	void (*registrarIntercambio)(struct ServicioDeMetricas* this, char* idDeUnEntrenadorInvolucrado);
	//Metodos privados
	int (*calcularCiclosTotales)(struct ServicioDeMetricas * this);
	void (*destruir)(struct ServicioDeMetricas * this);
} ServicioDeMetricas;

extern const struct ServicioDeMetricasClass {
	ServicioDeMetricas * (*new)();
} ServicioDeMetricasConstructor;

ServicioDeMetricas * servicioDeMetricasProcesoTeam;

#endif /* INCLUDE_SERVICIOS_SERVICIODEMETRICAS_SERVICIODEMETRICAS_H_ */
