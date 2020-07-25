//
// Created by Alan Zhao on 29/05/2020.
//

#ifndef TEAM_SERVICIODEPLANIFICACION_H
#define TEAM_SERVICIODEPLANIFICACION_H

#include <semaphore.h>
#include "app/Global.h"
#include "delibird/utils/hilos/HiloFacil.h"
#include "modelo/equipo/Equipo.h"
#include "planificador/Planificador.h"
#include "servicios/servicioDeResolucionDeDeadlocks/ServicioDeResolucionDeDeadlocks.h"
#include "servicios/servicioDeMetricas/ServicioDeMetricas.h"
#include "modelo/planificable/tarea/intercambio/TareaDeIntercambio.h"
#include "modelo/planificable/tarea/captura/TareaDeCaptura.h"
#include "modelo/objetivo/ObjetivoGlobal.h"

/**
 * Esta clase es conocedora de que implicancias a nivel planificacion tienen los eventos del sistema.
 *
 * Problemas:
 *  - Alguien tiene que persistir los ids, ¿Sera que puedo modelar los eventos y guardarlos ahi?
 *      Modelar eventos esperables y registrarlos en el event-handler.
 *  - Llego hasta tener al entrenador bloqueado esperando la respuesta del CATCH, ¿ya hago return void?
 *      Registro en el ManejadorDeEventos que necesito un CAUGHT y retorno.
 *
 * Responsabilidades:
 *  - Tiene conocimiento de las reglas de movimiento de los entrenadores en los diferentes estados.
 *  - Colaborar con la resolucion de deadlock facilitando mover entrenadores y realizar intercambios.
 *
 * ¿Que es un trabajo?
 *     - Tanto LOCALIZED como APPEARED involucran hacer las mismas acciones, asi que lo englobamos en CAPTURA_POKEMON.
 *     - CAUGHT: Agarra al entrenador correspondiente y le informa el resultado de su CATCH. -> NOTIFY_CAUGHT_RESULT
 *     - El intercambio por deadlock se va a representar con un DEADLOCK_RESOLUTION.
 *
 * Importante: Por el momento entendemos que si dejaron un trabajo es porque se puede cumplir.
 *
 * CAPTURA_POKEMON: Genera Tarea.
 *     Alguien mas arriba determinó que se puede capturar a cierto pokemon.
 *     Necesito tener a mano la ESPECIE y DONDE se encuentra.
 *     Al final genera una TareaDeCaptura.
 *
 * NOTIFY_CAUGHT_RESULT: No genera tarea.
 *     -- Esto podria hacerse en otro lado -- CapturaPokemonClass ponele.
 *     Tenemos que hacerle llegar al entrenador correspondiente el resultado de su CATCH.
 *     Si salio OK,
 *          Problema: Definir como guardar el intento de captura
 *          Solucion: Al momento de registrar que estamos esperando un caught, indicamos el pokemon y su posicion.
 *          mapa.eliminarPresencia(pokemon, posicion).
 *          entrenador.agregarPokemonCapturado(pikachu).
 *     Des-Registramos el evento esperado en el manejador de eventos.
 *     -- Esta parte afecta la planificacion --
 *     Habilitamos a que el entrenador sea nuevamente planificado.
 *
 * DEADLOCK_RESOLUTION: Genera Tarea.
 *     Nuestro algoritmo de detección encontró que hay DEADLOCK.
 *     El algoritmo de resolucion generará una o varias tareas de intercambio para solucionarlo.
 */

typedef enum TipoTrabajoPlanificador {
	CAPTURA_POKEMON, NOTIFY_CAUGHT_RESULT, // TODO: Volarlo
	DEADLOCK_RESOLUTION // TODO: Modelar el intercambio.
} TipoTrabajoPlanificador;

typedef struct TrabajoPlanificador {
	TipoTrabajoPlanificador tipo;
	char * objetivo;
	Coordinate coordenadaObjetivo;
} TrabajoPlanificador;

typedef struct ServicioDePlanificacion {
	t_log * logger;
	bool finDeTrabajo;
	sem_t semaforoFinDeTrabajo;
	sem_t semaforoEjecucionHabilitada;
	Planificador planificador;
	sem_t semaforoContadorColaDeTrabajo;
	Mapa mapa;
	ObjetivoGlobal objetivoGlobal;
	ServicioDeResolucionDeDeadlocks* servicioDeResolucionDeDeadlocks;
	ServicioDeMetricas* servicioDeMetricas;
	HiloEntrenadorPlanificable* ultimoHiloEjecutado;
	// Interfaz publica
	void (*trabajar)(struct ServicioDePlanificacion * this);
	void (*asignarEquipoAPlanificar)(struct ServicioDePlanificacion * this, Equipo equipo);
	void (*asignarTareasDeCaptura)(struct ServicioDePlanificacion * this, t_list* listaPokemon, t_list* entrenadoresDisponibles);
	void (*asignarIntercambios)(struct ServicioDePlanificacion * this, t_list* listaDeBloqueados);
	void (*definirYCambiarEstado)(struct ServicioDePlanificacion* this, UnidadPlanificable* hilo);
	bool (*teamFinalizado)(struct ServicioDePlanificacion* this);
	bool (*evaluarEstadoPosibleDeadlock)(struct ServicioDePlanificacion* this);
	void (*destruir)(struct ServicioDePlanificacion * this);

} ServicioDePlanificacion;

extern const struct ServicioDePlanificacionClass {
	ServicioDePlanificacion * (*new)(ServicioDeMetricas* servicioDeMetricas, ServicioDeResolucionDeDeadlocks* servicioDeadlocks);
} ServicioDePlanificacionConstructor;

ServicioDePlanificacion * servicioDePlanificacionProcesoTeam;

t_list * convertirAUnidadesPlanificables(Equipo equipo);
void destruirUnidadPlanificable(UnidadPlanificable * unidadPlanificable);
HiloEntrenadorPlanificable* devolverEntrenadorMasCercano(Coordinate coor, t_list* hilos);

#endif //TEAM_SERVICIODEPLANIFICACION_H
