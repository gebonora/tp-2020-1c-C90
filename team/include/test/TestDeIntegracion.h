//
// Created by Alan Zhao on 10/06/2020.
//

#ifndef TEAM_TESTDEINTEGRACION_H
#define TEAM_TESTDEINTEGRACION_H

#include <assert.h>
#include <unistd.h>
#include "app/Global.h"
#include "manejadorDeEventos/ManejadorDeEventos.h"
#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "planificador/algoritmos/fifo/FirstInFirstOut.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/mapa/registro/RegistradorDePosiciones.h"
#include "modelo/equipo/Equipo.h"
#include "modelo/objetivo/ObjetivoGlobal.h"
#include "modelo/planificable/tarea/captura/TareaDeCaptura.h"
#include "delibird/utils/strings/ExtensionStrings.h"
#include "delibird/utils/programadorDeTareas/ProgramadorDeTareas.h"
#include "modelo/mapa/movimiento/Movimiento.h"
#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"

void testDeIntegracion();
void testDeEntrenadores();
void testDeMovimiento();
void testDeMapa();
void testDeAlgoritmos();
void testDeTareas();
void testDePlanificable();
void testDeEventos();
void testServicioDePlanificacion();
void testDePlanificador();
void testDeTransicionadorDeEstados();

#endif //TEAM_TESTDEINTEGRACION_H
