//
// Created by Alan Zhao on 20/06/2020.
//

#include "modelo/mapa/Mapa.h"

char * registrarPosicion(Mapa * this, Coordinate posicion, TipoPosicionable tipoPosicionable) {
    char * posicionComoClave = coordenadaImprimible(posicion);
    Presencia * presencia = crearPresencia(tipoPosicionable);
    if (dictionary_has_key(this->plano, posicionComoClave)) {
        t_list * casilla = dictionary_get(this->plano, posicionComoClave);
        list_add(casilla, presencia);
    } else {
        t_list * casilla = list_create();
        list_add(casilla, presencia);
        dictionary_put(this->plano, coordenadaImprimible(posicion), casilla);
    }
    free(posicionComoClave);
    return presencia->uuid;
}

static void destruir(Mapa *this) {
    log_destroy(this->logger);
    //TODO: Vaciar plano correctamente
    dictionary_destroy(this->plano);
}

Plano crearPlano() {
    Plano plano = dictionary_create();
    return plano;
}

Presencia * crearPresencia(TipoPosicionable tipoPosicionable) {
    Presencia * presencia = malloc(sizeof(Presencia));
    presencia->uuid = generateUUID(30);
    presencia->tipoPosicionable = tipoPosicionable;
    return presencia;
}

char * coordenadaImprimible(Coordinate posicion) {
    return string_from_format("(%u,%u)", posicion.pos_x, posicion.pos_y);
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