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
#include "servicios/servicioDeMetricas/ServicioDeMetricas.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/equipo/Equipo.h"
#include "delibird/utils/colecciones/ExtensionColecciones.h"

/*
 * Esta clase se encarga de detección y resolución de deadlocks.
 * TODO:
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
 *  	NOTA IMPORTANTE: asumimos que un entrenador no va ser iniciado desde config con un pokemon que no necesita el equipo.
 *
 */

typedef t_list* ListaDeEntrenadores;
typedef t_list* ListaDeDependencias;
typedef t_list* ListaDeListaDeString;

typedef struct Intercambio { // Esto puede definir tambien que pokemon dan / reciben, o lo dejamos que lo determine el que genera la tarea (me gusta mas esto)
	char* entrenadorQueSeMueve;
	char* entrenadorQueEspera;
} Intercambio;

typedef struct Dependencias {
	char* nombreEntrenador;
	t_list* listaDependencias;
} Dependencias;

typedef struct ServicioDeResolucionDeDeadlocks {
	t_log * logger;
	bool primeraVez;
	ServicioDeMetricas* servicioDeMetricas;
	//Interfaz publica
	t_list* (*procesarDeadlock)(struct ServicioDeResolucionDeDeadlocks * this, t_list* entrenadoresBloqueados);
	//Metodos privados
	t_list* (*crearListaDeDependencias)(struct ServicioDeResolucionDeDeadlocks* this, ListaDeEntrenadores entrenadoresFiltrados);
	void (*detectarEnDetalleYLogear)(struct ServicioDeResolucionDeDeadlocks* this, ListaDeDependencias listaDeDependencias);
	t_list* (*resolverDeadlock)(struct ServicioDeResolucionDeDeadlocks * this, ListaDeDependencias listaDeDependencias);
	void (*destruir)(struct ServicioDeResolucionDeDeadlocks * this);
} ServicioDeResolucionDeDeadlocks;

extern const struct ServicioDeResolucionDeDeadlocksClass {
	ServicioDeResolucionDeDeadlocks * (*new)(ServicioDeMetricas* servicioDeMetricas);
} ServicioDeResolucionDeDeadlocksConstructor;

ServicioDeResolucionDeDeadlocks * servicioDeResolucionDeDeadlocksProcesoTeam;

// Funciones estáticas

bool hayDependencias(ListaDeStrings listaCapturas, ListaDeStrings listaObjetivos);
ListaDeStrings obtenerListaDePokemon(ContadorPokemones contador);
void imprimirListaStrings(ListaDeStrings lista);
bool hayDependencias(ListaDeStrings listaCapturas, ListaDeStrings listaObjetivos);
void imprimirListaDeListas(ListaDeListaDeString listaDeListas);
void imprimirDependencias(Dependencias* dependencias);
void imprimirListaDeDependencias(ListaDeDependencias listaDeDependencias);
bool hayDependenciaEnComun(Dependencias* dependencia1, Dependencias* dependencia2);
ListaDeStrings eliminarListasRepetidas(ListaDeStrings listaDeListas);
void destruirDependencia(void * elem);
void destruirIntercambio(void* aDestruir);
char* obtenerReporteDeadlocks(t_list* deadlocks);
ListaDeListaDeString sumarListasSiHayMatch(ListaDeListaDeString listaDeListas);

#endif /* INCLUDE_SERVICIOS_SERVICIODERESOLUCIONDEDEADLOCKS_SERVICIODERESOLUCIONDEDEADLOCKS_H_ */
