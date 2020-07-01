//
// Created by Alan Zhao on 10/06/2020.
//

#ifndef TEAM_TESTDEINTEGRACION_H
#define TEAM_TESTDEINTEGRACION_H

#include <assert.h>
#include "manejadorDeEventos/ManejadorDeEventos.h"
#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "planificador/algoritmos/fifo/FirstInFirstOut.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/mapa/registro/RegistradorDePosiciones.h"
#include "modelo/equipo/Equipo.h"
#include "modelo/objetivo/ObjetivoGlobal.h"
#include "delibird/utils/strings/ExtensionStrings.h"
#include "delibird/utils/programadorDeTareas/ProgramadorDeTareas.h"
#include <unistd.h>"

void testDeIntegracion();
void testDeEntrenadores();
void testDeMapa();
void testDeAlgoritmos();
void testDePlanificacion();
void testDeEventos();

#endif //TEAM_TESTDEINTEGRACION_H
