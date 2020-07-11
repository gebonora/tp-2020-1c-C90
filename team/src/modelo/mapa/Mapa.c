//
// Created by Alan Zhao on 20/06/2020.
//

#include "modelo/mapa/Mapa.h"

#define filtrarPormismoUUID(uuid) \
bool mismoUUID(void * _posiblePresencia) { \
    Presencia * posiblePresencia = _posiblePresencia; \
    return string_equals(posiblePresencia->uuid, uuid); \
}

void moverPosicionable(Mapa * this, char * uuid, Coordinate destino) {
    char * posicionDestino = coordenadaClave(destino);
    log_debug(this->logger, "Moviendo a posicion %s el posicionable %s", posicionDestino, uuid);
    Posicion posicionActual = this->obtenerPosicion(this, uuid);
    if (posicionActual.valida) {
        char * posicionComoClave = coordenadaClave(posicionActual.coordenada);
        Casilla casillaActual = dictionary_get(this->plano, posicionComoClave);
        free(posicionComoClave);
        filtrarPormismoUUID(uuid);
        Presencia * presencia = list_remove_by_condition(casillaActual, mismoUUID);
        this->agregarPresenciaACasillaExistenteOCrearUna(this, posicionDestino, presencia);
    } else {
        log_error(this->logger, "Movimiento cancelado. No se pudo encontrar en el mapa a %s", uuid);
    }
    free(posicionDestino);
    this->dibujarMapa(this);
}

void agregarPresenciaACasillaExistenteOCrearUna(Mapa * this, char * posicion, Presencia * presencia) {
    if (dictionary_has_key(this->plano, posicion)) {
        log_debug(this->logger, "Ya existe la posición %s en el mapa. Agregamos la presencia a la casilla.", posicion);
        Casilla casillaObjetivo = dictionary_get(this->plano, posicion);
        list_add(casillaObjetivo, presencia);
    } else {
        log_debug(this->logger, "No existe la posicion %s en el mapa. Creamos una nueva casilla.", posicion);
        Casilla casillaObjetivo = crearCasilla();
        list_add(casillaObjetivo, presencia);
        dictionary_put(this->plano, posicion, casillaObjetivo);
    }
}

char * registrarPosicion(Mapa * this, Coordinate posicion, TipoPosicionable tipoPosicionable) {
    char * posicionComoClave = coordenadaClave(posicion);
    Presencia * presencia = crearPresencia(tipoPosicionable);
    this->agregarPresenciaACasillaExistenteOCrearUna(this, posicionComoClave, presencia);
    log_debug(this->logger, "Se registró un %s en la posición %s", nombreTipoPosicionable(tipoPosicionable), posicionComoClave);
    free(posicionComoClave);
    this->dibujarMapa(this);
    return presencia->uuid;
}

Posicion obtenerPosicion(Mapa * this, char * uuid) {
    Posicion posicion = {.valida=false};

    filtrarPormismoUUID(uuid);

    void posicionTienePresenciaConMismoUUID(char * coordenadaClave, void * casilla) {
        Presencia * posiblePresencia = list_find(casilla, mismoUUID);
        if (posiblePresencia != NULL) {
            posicion.valida=true;
            posicion.coordenada = convertirClaveACoordenada(coordenadaClave);
        }
    }

    dictionary_iterator(this->plano, posicionTienePresenciaConMismoUUID);

    return posicion;
}

static void dibujarMapa(Mapa * this) {
    t_list * coordenadasClave = (t_list *) dictionary_keys(this->plano);
    uint32_t max = 0;
    for (int i = 0; i < list_size(coordenadasClave); i++) {
        char * coordenadaClave = (char *) list_get(coordenadasClave, i);
        Coordinate coordenada = convertirClaveACoordenada(coordenadaClave);
        if (coordenada.pos_x > max) {
            max = coordenada.pos_x;
        }
        if (coordenada.pos_y > max) {
            max = coordenada.pos_y;
        }
    }
    list_destroy(coordenadasClave);

    char * separadorDibujo = string_new();
    for (int s = 0; s < ((max * 3) + 1) / 2; s++) {
        string_append(&separadorDibujo, "#");
    }
    log_debug(this->logger, "%s Mapa %s", separadorDibujo, separadorDibujo);

    char * enumeracionColumnas = string_from_format("   ");
    for (int e = 0; e <= max; e++) {
        string_append_with_format(&enumeracionColumnas, " %-2d", e);
    }
    log_debug(this->logger, enumeracionColumnas);
    free(enumeracionColumnas);

    for (int x = 0; x <= max; x++) {
        char * arriba = string_new();
        char * medio = string_new();
        string_append(&arriba, "   ");
        string_append_with_format(&medio, "%-3d", x);
        for (int y = 0; y <= max; y++) {
            string_append(&arriba, "+--");

            char * representacionPresencia = string_new();
            char * coordenadaClave = armarCoordenadaClave(x,y);

            if (dictionary_has_key(this->plano, coordenadaClave)) {
                Casilla casilla = dictionary_get(this->plano, coordenadaClave);
                for (int p = 0; p < list_size(casilla); p++) {
                    Presencia * presencia = list_get(casilla, p);
                    char * inicialPresencia = string_substring(nombreTipoPosicionable(presencia->tipoPosicionable), 0, 1);
                    if (!string_contains(representacionPresencia, inicialPresencia)) {
                        string_append(&representacionPresencia, inicialPresencia);
                    }
                    free(inicialPresencia);
                }
            }
            string_append_with_format(&medio, "|%-2s", representacionPresencia);
            free(coordenadaClave);
            free(representacionPresencia);
        }
        string_append(&arriba, "+");
        string_append(&medio, "|");
        log_debug(this->logger, arriba);
        log_debug(this->logger, medio);
        free(arriba);
        free(medio);
    }

    char * final = string_new();
    string_append(&final, "   ");
    for (int b = 0; b <= max; b ++) {
        string_append(&final, "+--");
    }
    string_append(&final, "+");
    log_debug(this->logger, final);
    free(final);

    log_debug(this->logger, "%s Mapa %s", separadorDibujo, separadorDibujo);
    free(separadorDibujo);
}

static void destruir(Mapa *this) {
    log_debug(this->logger, "Se procede a destruir el mapa");
    log_destroy(this->logger);
    dictionary_destroy_and_destroy_elements(this->plano, (void (*)(void *)) destruirCasilla);
}

static Mapa new() {
    return (Mapa) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "Mapa", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
            .plano = crearPlano(),
            &moverPosicionable,
            &agregarPresenciaACasillaExistenteOCrearUna,
            &registrarPosicion,
            &obtenerPosicion,
            &dibujarMapa,
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