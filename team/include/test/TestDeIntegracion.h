//
// Created by Alan Zhao on 10/06/2020.
//

#ifndef TEAM_TESTDEINTEGRACION_H
#define TEAM_TESTDEINTEGRACION_H

#include <assert.h>
#include <unistd.h>
#include "app/Global.h"
#include "modelo/planificable/tarea/intercambio/TareaDeIntercambio.h"
#include "modelo/planificable/tarea/captura/TareaDeCaptura.h"
#include "manejadorDeEventos/ManejadorDeEventos.h"
#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "planificador/algoritmos/fifo/FirstInFirstOut.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/mapa/registro/RegistradorDePosiciones.h"
#include "modelo/equipo/Equipo.h"
#include "modelo/objetivo/ObjetivoGlobal.h"
#include "delibird/utils/strings/ExtensionStrings.h"
#include "delibird/utils/programadorDeTareas/ProgramadorDeTareas.h"
#include "modelo/mapa/movimiento/Movimiento.h"
#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"
#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"
#include "servicios/servicioDeMetricas/ServicioDeMetricas.h"
#include "cliente/ClienteBrokerV2.h"
#include "manejadorDeEventos/registro/RegistradorDeEventos.h"
#include "servicios/servicioDeResolucionDeDeadlocks/ServicioDeResolucionDeDeadlocks.h"

void testDeIntegracion();
void testDeEntrenadores();
void testDePokemones();
void testDeMovimiento();
void testDeMapa();
void testDeAlgoritmos();
void testDeTareas();
void testDePlanificable();
void testDeEventos();
void testServicioDePlanificacion();
void testDePlanificador();
void testDeTransicionadorDeEstados();
void testServicioDeCaptura();
void testLibs();
void testDeAlgoritmosSJFsinDesalojo();
void testDeServicioDeMetricas();
void testClienteBroker();
void testDeadlock();
void testDeIntercambio();

#endif //TEAM_TESTDEINTEGRACION_H
