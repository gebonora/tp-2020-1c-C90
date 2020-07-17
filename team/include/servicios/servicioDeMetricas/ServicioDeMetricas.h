/*
 * ServicioDeMetricas.h
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#ifndef INCLUDE_SERVICIOS_SERVICIODEMETRICAS_SERVICIODEMETRICAS_H_
#define INCLUDE_SERVICIOS_SERVICIODEMETRICAS_SERVICIODEMETRICAS_H_

#include "app/Global.h"

/*
 * Esta clase se encarga de registrar metricas e imprimirlas.
 *
 * TODO:
 * 		Definir dependencias, variables y metodos.
 *
 */

typedef struct ServicioDeMetricas {
	t_log * logger;
	int cantidadCambiosDeContexto;
	int cantidadDeadlocksProducidos;
	int cantidadDeadlocksResueltos;
	//Interfaz publica
	void (*foo)(struct ServicioDeMetricas * this);
	//Metodos privados
	void (*bar)(struct ServicioDeMetricas * this);
	void (*destruir)(struct ServicioDeMetricas * this);
} ServicioDeMetricas;

extern const struct ServicioDeMetricasClass {
	ServicioDeMetricas * (*new)();
} ServicioDeMetricasConstructor;

ServicioDeMetricas * servicioDeMetricasProcesoTeam;

#endif /* INCLUDE_SERVICIOS_SERVICIODEMETRICAS_SERVICIODEMETRICAS_H_ */
