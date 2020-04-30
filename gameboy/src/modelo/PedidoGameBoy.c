//
// Created by Alan Zhao on 30/04/2020.
//

#include "modelo/PedidoGameBoy.h"

PedidoGameBoy *crearPedidoGameBoy(int cantidadArugmentos, char** arrayArgumentos) {
    PedidoGameBoy *pedidoGameBoy = malloc(sizeof(PedidoGameBoy));

    Proceso proceso = obtenerProceso(arrayArgumentos[0]);
    pedidoGameBoy->proceso = proceso;

    int inicioArgumentos;

    if (proceso == SUSCRIPTOR) {
        pedidoGameBoy->tipoMensaje = UNTYPED_MESSAGE;
        inicioArgumentos = 1;
    } else {
        pedidoGameBoy->tipoMensaje = obtenerTipoMensaje(arrayArgumentos[1]);
        inicioArgumentos = 2;
    }

    Argumentos argumentos = list_create();
    for (int i=inicioArgumentos; i < cantidadArugmentos; i++)
        list_add(argumentos, arrayArgumentos[i]);

    pedidoGameBoy->argumentos = argumentos;

    return pedidoGameBoy;
}

Proceso obtenerProceso(char * nombreProceso) {
    if (string_equals_ignore_case("BROKER", nombreProceso)) {
        return BROKER;
    } else if (string_equals_ignore_case("TEAM", nombreProceso)) {
        return TEAM;
    } else if (string_equals_ignore_case("CATCH_POKEMON", nombreProceso)) {
        return CATCH_POKEMON;
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
    } else {
        log_error(INTERNAL_LOGGER, "Tipo de mensaje no soportado: %s", tipoMensaje);
        exit(EXIT_FAILURE);
    }
}

void eliminarPedidoGameBoy(PedidoGameBoy *pedidoGameBoy) {
    list_destroy(pedidoGameBoy->argumentos);
    free(pedidoGameBoy);
}