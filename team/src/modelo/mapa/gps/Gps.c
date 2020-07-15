//
// Created by Alan Zhao on 25/06/2020.
//

#include "modelo/mapa/gps/Gps.h"

bool borrarPosicionDelMapa(Gps * this) {
    //TODO
    return true;
}

Posicion posicionActual(Gps * this) {
    Mapa * mapa = this->mapa;
    Posicion posicion = mapa->obtenerPosicion(mapa, this->uuid);
    if (posicion.valida == false) {
        log_error(this->logger, "No se pudo encontrar la posicion asociada al uuid %s", this->uuid);
    }
    return posicion;
}

void irA(Gps * this, Coordinate destino) {
    char * coordenada = coordenadaClave(destino);
    log_debug(this->logger, "Guiando posicionable %s hacia %s", this->uuid, coordenada);
    free(coordenada);
    Mapa * mapa = this->mapa;
    mapa->moverPosicionable(mapa, this->uuid, destino);
}

void destruirGps(Gps * this) {
    log_destroy(this->logger);
    free(this->uuid);
    free(this);
}

static Gps * new(Mapa * mapaDeRegistro, char * uuid) {
    Gps * gps = malloc(sizeof(Gps));

    gps->logger = log_create(TEAM_INTERNAL_LOG_FILE, "Gps", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
    gps->uuid = uuid;
    gps->mapa = mapaDeRegistro;
    gps->posicionActual = &posicionActual;
    gps->irA = &irA;
    gps->borrarPosicionDelMapa = &borrarPosicionDelMapa;
    gps->destruirGps = &destruirGps;

    return gps;
}

const struct GpsClass GpsConstructor = {.new=&new};