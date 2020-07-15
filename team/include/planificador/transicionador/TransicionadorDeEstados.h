//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_TRANSICIONADORDEESTADOS_H
#define TEAM_TRANSICIONADORDEESTADOS_H

#include "app/Global.h"
#include "planificador/estados/EstadoPlanificador.h"
#include "planificador/colas/ColasDePlanificacion.h"
#include "planificador/algoritmos/AlgoritmoPlanificador.h"

/**
 * Esta clase se va a encargar de transicionar el estado de la unidad planificable (UP).
 * Garantiza que no haya transiciones ilegales (ver State Pattern).
 * Criterios para entrar a:
 *  - NEW: Al comenzar el proceso Team, todos los entrenadores instanciados deben colocarse en esta cola.
 *  - READY: No tenes que estar haciendo NADA, es decir, estas en NEW o BLOCKED (¡Causado por INACTIVIDAD!).
 *  - EXEC: Entrenador mas cercano al pokemon.
 */

typedef enum {
    TRANSICION_EXITOSA,
    TRANSICION_FALLIDA
} ResultadoTransicion;

typedef struct TransicionadorDeEstados {
    t_log *logger;
    ResultadoTransicion (*transicionarA)(struct TransicionadorDeEstados *this, UnidadPlanificable * unidadPlanificable, EstadoPlanificador estadoDeseado);
    void (*destruir)(struct TransicionadorDeEstados *this);
} TransicionadorDeEstados;

extern const struct TransicionadorDeEstadosClass {
    TransicionadorDeEstados (*new)();
} TransicionadorDeEstadosConstructor;

bool estadoPrevioSatisfecho(EstadoPlanificador estadoPrevio, EstadoPlanificador estadoDeseado);

#endif //TEAM_TRANSICIONADORDEESTADOS_H
