//
// Created by Alan Zhao on 20/07/2020.
//

#include "cliente/ClienteBrokerV2.h"

static RespuestaBroker enviarGet(ClienteBrokerV2 * this, char* nombrePokemon) {
    RespuestaBroker respuestaBroker = {.esValida = false};
    int socketDescartable = crearSocketCliente(this->ipBroker, this->puertoBroker);
    int flagBrokerCaido = 0;

    Get* unGet = create_get_pokemon(nombrePokemon);

    if (send_get(unGet, socketDescartable) < 0) {
        flagBrokerCaido = 1;
    }

    uint32_t idAsignado;

    if (recv(socketDescartable, &idAsignado, sizeof(uint32_t), 0) <= 0) {
        flagBrokerCaido = 1;
    }

    if (!flagBrokerCaido) {
        log_info(MANDATORY_LOGGER, "Se envió al Broker un GET pokemon: %s. Se le asignó el idMensaje: %d.",
                 nombrePokemon, idAsignado);
        return (RespuestaBroker) {.esValida = true, .idCorrelatividad = idAsignado};
    } else {
        log_warning(MANDATORY_LOGGER, "No se pudo enviar un GET pokemon: %s. Se procede a asumir que no hay coordenadas para el pokemon.", nombrePokemon);
        return (RespuestaBroker) {.esValida = false, .idCorrelatividad = 0};
    }
}

static RespuestaBroker enviarCatch(ClienteBrokerV2 * this, char * especie, uint32_t posX, uint32_t posY) {
    int socketDescartable = crearSocketCliente(this->ipBroker, this->puertoBroker);
    int flagBrokerCaido = 0;

    Pokemon* unPokemon = create_pokemon(especie, posX, posY);

    if (send_pokemon(unPokemon, socketDescartable, false) < 0) {
        flagBrokerCaido = 1;
    }

    uint32_t idAsignado;

    if (recv(socketDescartable, &idAsignado, sizeof(uint32_t), 0) <= 0) {
        flagBrokerCaido = 1;
    }

    if (!flagBrokerCaido) {
        log_info(MANDATORY_LOGGER,
                 "Se envió al Broker un CATCH pokemon: %s, coordenadas: (%d,%d). Se le asignó el idMensaje: %d.",
                 especie, posX, posY, idAsignado);
        return (RespuestaBroker) {.esValida = true, .idCorrelatividad = idAsignado};
    } else {
        log_warning(MANDATORY_LOGGER, "No se puedo enviar un CATCH pokemon: %s, coordenadas: (%d,%d)", especie, posX, posY);
        return (RespuestaBroker) {.esValida = false};
    }
}

static void destruir(ClienteBrokerV2 * this) {
    log_debug(this->logger, "Se procede a destruir el Cliente Broker");
    log_destroy(this->logger);
    free(this);
}

static ClienteBrokerV2 * new() {
    ClienteBrokerV2 * clienteBroker = malloc(sizeof(ClienteBrokerV2));

    clienteBroker->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ClienteBrokerV2", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
    clienteBroker->ipBroker = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_BROKER);
    clienteBroker->puertoBroker = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_BROKER);
    clienteBroker->enviarCatch=&enviarCatch;
    clienteBroker->enviarGet=&enviarGet;
    clienteBroker->destruir = &destruir;

    return clienteBroker;
}

const struct ClienteBrokerV2Class ClienteBrokerV2Constructor = {.new=&new};
