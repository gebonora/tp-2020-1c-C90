//
// Created by Alan Zhao on 25/06/2020.
//

#ifndef TEAM_GPS_H
#define TEAM_GPS_H

#include "app/Global.h"
#include "modelo/mapa/Mapa.h"

typedef struct Gps {
    t_log * logger;
    char * uuid;
    Mapa * mapa;
    //Interfaz publica
    Posicion (*posicionActual)(struct Gps * this);
    void (*irA)(struct Gps * this, Coordinate destino);
    void (*destruirGps)(struct Gps * this);
} Gps;

extern const struct GpsClass {
    Gps * (*new)(Mapa * mapa, char * uuid);
} GpsConstructor;

#endif //TEAM_GPS_H
