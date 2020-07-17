//
// Created by Alan Zhao on 07/05/2020.
//

#ifndef TEAM_APP_H
#define TEAM_APP_H

#include "app/Global.h"
#include "app/Servicios.h"
#include "servidor/ServidorTeam.h"
#include "modelo/equipo/Equipo.h"
#include "modelo/objetivo/ObjetivoGlobal.h"
#include "test/TestDeIntegracion.h"
#include "manejadorDeEventos/ManejadorDeEventos.h"

void warmUp();
void mostrarTitulo(t_log * logger);
void inicializarComponentesDelSistema();
void configurarEstadoInicialProcesoTeam();
void liberarRecursos();

#endif //TEAM_APP_H
