//
// Created by Alan Zhao on 15/07/2020.
//

#ifndef TEAM_SERVICIODECAPTURA_H
#define TEAM_SERVICIODECAPTURA_H

#include "app/Global.h"
#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/equipo/Equipo.h"

typedef struct ServicioDeCaptura {
    t_log * logger;
    Mapa mapa;
    Equipo equipo;
    ServicioDePlanificacion * servicioDePlanificacion;
    void (*destruir)(struct ServicioDeCaptura * this);
} ServicioDeCaptura;

extern const struct ServicioDeCapturaClass {
    ServicioDeCaptura * (*new)(Mapa mapa, Equipo equipo, ServicioDePlanificacion * servicioDePlanificacion);
} ServicioDeCapturaConstructor;

ServicioDeCaptura * servicioDeCapturaProcesoTeam;

#endif //TEAM_SERVICIODECAPTURA_H
