//
// Created by Alan Zhao on 30/04/2020.
//

#include "modelo/PedidoGameBoy.h"

PedidoGameBoy crearPedidoGameBoy(int cantidadArugmentos, char** arrayArgumentos) {
    PedidoGameBoy pedidoGameBoy = {.argumentos=malloc(sizeof(Argumentos))};

    Process proceso = obtenerProceso(arrayArgumentos[1]);
    pedidoGameBoy.proceso = proceso;

    int inicioArgumentos = 3;

    if (proceso == SUSCRIPTOR) {
        pedidoGameBoy.tipoMensaje = SUBSCRIBE;
        inicioArgumentos = 2;
    } else {
        pedidoGameBoy.tipoMensaje = obtenerTipoMensaje(arrayArgumentos[2]);
    }

    Argumentos argumentos = list_create();
    for (int i=inicioArgumentos; i < cantidadArugmentos; i++)
        list_add(argumentos, arrayArgumentos[i]);

    pedidoGameBoy.argumentos = argumentos;

    return pedidoGameBoy;
}

Process obtenerProceso(char * nombreProceso) {
    if (string_equals_ignore_case("BROKER", nombreProceso)) {
        return BROKER;
    } else if (string_equals_ignore_case("TEAM", nombreProceso)) {
        return TEAM;
    } else if (string_equals_ignore_case("GAMECARD", nombreProceso)) {
        return GAMECARD;
    } else if (string_equals_ignore_case("SUSCRIPTOR", nombreProceso)) {
        return SUSCRIPTOR;
    } else {
        log_error(INTERNAL_LOGGER, "Proceso no soportado: %s", nombreProceso);
        exit(EXIT_FAILURE);
    }
}

TipoMensaje obtenerTipoMensaje(char * tipoMensaje) {
    if (string_equals_ignore_case("NEW_POKEMON", tipoMensaje)) {
        return NEW_POKEMON;
    } else if (string_equals_ignore_case("APPEARED_POKEMON", tipoMensaje)) {
        return APPEARED_POKEMON;
    } else if (string_equals_ignore_case("CATCH_POKEMON", tipoMensaje)) {
        return CATCH_POKEMON;
    } else if (string_equals_ignore_case("CAUGHT_POKEMON", tipoMensaje)) {
        return CAUGHT_POKEMON;
    } else if (string_equals_ignore_case("GET_POKEMON", tipoMensaje)) {
        return GET_POKEMON;
    } else if (string_equals_ignore_case("LOCALIZED_POKEMON", tipoMensaje)) {
        return LOCALIZED_POKEMON;
    } else {
        log_error(INTERNAL_LOGGER, "Tipo de mensaje no soportado: %s", tipoMensaje);
        exit(EXIT_FAILURE);
    }
}

char * nombreProceso(Process proceso) {
    switch (proceso) {
        case BROKER:
            return "BROKER";
        case TEAM:
            return "TEAM";
        case GAMECARD:
            return "GAMECARD";
        case SUSCRIPTOR:
            return "SUSCRIPTOR";
        default:
            return "UNKNOWN_PROCESS";
    }
}

char * nombreTipoMensaje(TipoMensaje tipoMensaje) {
    switch (tipoMensaje) {
        case NEW_POKEMON:
            return "NEW_POKEMON";
        case APPEARED_POKEMON:
            return "APPEARED_POKEMON";
        case CATCH_POKEMON:
            return "CATCH_POKEMON";
        case CAUGHT_POKEMON:
            return "CAUGHT_POKEMON";
        case GET_POKEMON:
            return "GET_POKEMON";
        default:
            return "UNKNOWN_MESSAGE";
    }
}

void eliminarPedidoGameBoy(PedidoGameBoy *pedidoGameBoy) {
    list_destroy(pedidoGameBoy->argumentos);
    free(pedidoGameBoy);
}
