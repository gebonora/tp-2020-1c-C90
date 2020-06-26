//
// Created by Alan Zhao on 25/06/2020.
//

#include "modelo/mapa/gps/Gps.h"

Posicion posicionActual(Gps * this) {
    Mapa * mapa = this->mapa;
    return mapa->obtenerPosicion(mapa, this->uuid);
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
    gps->posicionActual = &posicionActual;
    gps->destruir = &destruir;

    return gps;
}

const struct GpsClass GpsConstructor = {.new=&new};