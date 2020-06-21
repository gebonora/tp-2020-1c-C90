//
// Created by Alan Zhao on 20/06/2020.
//

#include "modelo/mapa/Mapa.h"

char * registrarPosicion(Mapa * this, Coordinate posicion, Posicionable posicionable) {
    return generateUUID(30);
}

static void destruir(Mapa *this) {
    log_destroy(this->logger);
    dictionary_destroy(this->plano);
}

Plano crearPlano() {
    Plano plano = dictionary_create();
    return plano;
}

static Mapa new() {
    return (Mapa) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "Mapa", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO),
            .plano = crearPlano(),
            &registrarPosicion,
            &destruir
    };
}

const struct MapaClass MapaConstructor = {.new=&new};