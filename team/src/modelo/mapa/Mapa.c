//
// Created by Alan Zhao on 20/06/2020.
//

#include "modelo/mapa/Mapa.h"

char * registrarPosicion(Mapa * this, Coordinate posicion, TipoPosicionable tipoPosicionable) {
    char * posicionComoClave = coordenadaImprimible(posicion);
    Presencia * presencia = crearPresencia(tipoPosicionable);
    if (dictionary_has_key(this->plano, posicionComoClave)) {
        Casilla casilla = dictionary_get(this->plano, posicionComoClave);
        list_add(casilla, presencia);
    } else {
        Casilla casilla = crearCasilla();
        list_add(casilla, presencia);
        dictionary_put(this->plano, posicionComoClave, casilla);
    }
    log_info(this->logger, "Se registró un %s en la posición %s", nombreTipoPosicionable(tipoPosicionable), posicionComoClave);
    free(posicionComoClave);
    return presencia->uuid;
}

static void destruir(Mapa *this) {
    log_debug(this->logger, "Se procede a destruir el mapa");
    log_destroy(this->logger);
    dictionary_destroy_and_destroy_elements(this->plano, (void (*)(void *)) destruirCasilla);
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

// Funciones estaticas
Plano crearPlano() {
    Plano plano = dictionary_create();
    return plano;
}

Casilla crearCasilla() {
    Casilla casilla = list_create();
    return casilla;
}

Presencia * crearPresencia(TipoPosicionable tipoPosicionable) {
    Presencia * presencia = malloc(sizeof(Presencia));
    presencia->uuid = generateUUID(30);
    presencia->tipoPosicionable = tipoPosicionable;
    return presencia;
}

void destruirCasilla(Casilla casilla) {
    list_destroy_and_destroy_elements(casilla, (void (*)(void *)) destruirPresencia);
}

void destruirPresencia(Presencia * presencia) {
    free(presencia->uuid);
    free(presencia);
}

char * nombreTipoPosicionable(TipoPosicionable posicionable) {
    switch (posicionable) {
        case ENTRENADOR:
            return "ENTRENADOR";
        case POKEMON:
            return "POKEMON";
        default:
            return "POSICIONABLE DESCONOCIDO";
    }
}

char * coordenadaImprimible(Coordinate posicion) {
    return string_from_format("(%u,%u)", posicion.pos_x, posicion.pos_y);
}