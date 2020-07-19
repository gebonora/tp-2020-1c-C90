/*
 * ServicioDeResolucionDeDeadlocks.h
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#ifndef INCLUDE_SERVICIOS_SERVICIODERESOLUCIONDEDEADLOCKS_SERVICIODERESOLUCIONDEDEADLOCKS_H_
#define INCLUDE_SERVICIOS_SERVICIODERESOLUCIONDEDEADLOCKS_SERVICIODERESOLUCIONDEDEADLOCKS_H_

#include "app/Global.h"
#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/equipo/Equipo.h"

/*
 * Esta clase se encarga de detección y resolución de deadlocks.
 * TODO:
 * 		Definir dependencias.
 *  	Implementar algoritmo de deteccion.
 *  	Implementar algoritmo para determinar si se puede resolver deadlock.
 *  	Implementar algoritmo para resolver deadlock.
 *
 *  Posibles soluciones.
 *
 *  	DETECCION: un entrenador está en deadlock cuando está lleno y no cumplió su objetivo personal.
 *  	Entonces apuntemos a correr el algoritmo de detección cuando un entrenador se llena.
 *
 *  	RESOLUCION: sera intercambiar pokemon entre entrenadores hasta que cada uno tenga los que necesite.
 *  	Cuando un entrenador esta lleno y no tiene los pokemon que necesita, va a quedar en estado bloqueado.
 *  	Hay que analizar los pokemon en este estado para determinar si hay deadlock, y luego analizar si haciendo intercambios se lo puede resolver.
 *  	Como el team captura lo que necesita, el maximo deadlock posible es de todos los entrenadores, que se da al capturar todos los pokemon,
 *  	tiene solucion realizando intercambios.
 *
 */

typedef struct ServicioDeResolucionDeDeadlocks {
	t_log * logger;
	//Interfaz publica
	bool (*detectarDeadlock)(struct ServicioDeResolucionDeDeadlocks * this, t_list* entrenadoresBloqueados);
	void (*puedeResolverDeadlock)(struct ServicioDeResolucionDeDeadlocks * this);
	void (*resolverDeadlock)(struct ServicioDeResolucionDeDeadlocks * this);
	//Metodos privados
	void (*foo)(struct ServicioDeResolucionDeDeadlocks * this);
	void (*destruir)(struct ServicioDeResolucionDeDeadlocks * this);
} ServicioDeResolucionDeDeadlocks;

extern const struct ServicioDeResolucionDeDeadlocksClass {
	ServicioDeResolucionDeDeadlocks * (*new)();
} ServicioDeResolucionDeDeadlocksConstructor;

ServicioDeResolucionDeDeadlocks * servicioDeResolucionDeDeadlocksProcesoTeam;

// Funciones estáticas

bool hayDependencias(t_list* listaCapturas, t_list* listaObjetivos);
t_list* obtenerListaDePokemon(ContadorPokemones contador);
void imprimirListaStrings(t_list* lista);
t_list* restarPrimerListaASegunda(t_list* lista1, t_list* lista2);
t_list* copiarListaYElementos(t_list* lista);

#endif /* INCLUDE_SERVICIOS_SERVICIODERESOLUCIONDEDEADLOCKS_SERVICIODERESOLUCIONDEDEADLOCKS_H_ */
