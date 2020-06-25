//
// Created by Alan Zhao on 25/06/2020.
//

#include "modelo/mapa/gps/Gps.h"

Coordinate posicion(Gps * this) {
    Coordinate coordinate = {.pos_x = 0, .pos_y = 0};
    return coordinate;
}

void destruir(Gps * this) {
    log_destroy(this->logger);
    free(this->uuid);
    free(this);
}

static Gps * new(Mapa * mapaDeRegistro, char * uuid) {
    Gps * gps = malloc(sizeof(Gps));

    gps->logger = log_create(TEAM_INTERNAL_LOG_FILE, "Gps", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    gps->uuid = uuid;
    gps->mapa = mapaDeRegistro;
    gps->posicion = &posicion;
    gps->destruir = &destruir;

    return gps;
}

const struct GpsClass GpsConstructor = {.new=&new};