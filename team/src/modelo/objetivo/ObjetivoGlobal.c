//
// Created by Alan Zhao on 26/06/2020.
//

#include "modelo/objetivo/ObjetivoGlobal.h"

t_list * especiesNecesarias(ObjetivoGlobal * this) {
    return (t_list *) dictionary_keys(this->contabilidadEspecies);
}

bool puedeCapturarse(ObjetivoGlobal * this, char * especiePokemon) {
    if (dictionary_has_key(this->contabilidadEspecies, especiePokemon)) {
        ContabilidadEspecie * contabilidadEspecie = dictionary_get(this->contabilidadEspecies, especiePokemon);
        return contabilidadEspecie->necesarios > contabilidadEspecie->capturados;
    }
    return false;
}

void imprimirObjetivoGlobal(ObjetivoGlobal * this) {
    void agregarEntradaAString(char * pokemon, void * _contabilidad) {
        ContabilidadEspecie * contabilidadEspecie = _contabilidad;
        log_debug(this->logger, "%-15s %-15d %-15d", pokemon, contabilidadEspecie->necesarios, contabilidadEspecie->capturados);
    }
    log_debug(this->logger, "------------------------------------------");
    log_debug(this->logger, "              Objetivo Global             ");
    log_debug(this->logger, "------------------------------------------");
    log_debug(this->logger, "%-16s%-16s%-16s", "Especie", "Necesarios", "Capturados");
    log_debug(this->logger, "------------------------------------------");
    dictionary_iterator(this->contabilidadEspecies, agregarEntradaAString);
    log_debug(this->logger, "------------------------------------------");
}

void solicitarPokemones(ObjetivoGlobal * this) {
    t_list * especiesNecesarias = this->especiesNecesarias(this);

    for (int i = 0; i < list_size(especiesNecesarias); i++) {
        char * pokemon = list_get(especiesNecesarias, i);
        log_debug(this->logger, "Solicitando especie: %s", pokemon);
        enviarGet(pokemon);
    }

    list_destroy(especiesNecesarias);
}

void destruirObjetivoGlobal(ObjetivoGlobal * this) {
    log_debug(this->logger, "Se procede a destruir el objetivo global");
    log_destroy(this->logger);
    dictionary_destroy_and_destroy_elements(this->contabilidadEspecies, free);
}

static ObjetivoGlobal new(Equipo unEquipo) {
    ObjetivoGlobal objetivo = {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "ObjetivoGlobal", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
            .contabilidadEspecies = calcularObjetivoEspecies(unEquipo),
            &especiesNecesarias,
            &puedeCapturarse,
            &imprimirObjetivoGlobal,
            &solicitarPokemones,
            &destruirObjetivoGlobal
    };
    objetivo.imprimirObjetivoGlobal(&objetivo);
    return objetivo;
}

const struct ObjetivoGlobalClass ObjetivoGlobalConstructor = {.new=&new};

t_dictionary * calcularObjetivoEspecies(Equipo entrenadores) {
    t_dictionary * contabilidadEspecies = dictionary_create();

    void agregarPokemonesNecesarios(char * nombreEspecie, void * cantidad) {
        if (dictionary_has_key(contabilidadEspecies, nombreEspecie)) {
            ContabilidadEspecie * contabilidadEspecie = dictionary_get(contabilidadEspecies, nombreEspecie);
            contabilidadEspecie->necesarios += (int) cantidad;
        } else {
            ContabilidadEspecie * contabilidadEspecie = crearContabilidadEspecie((int) cantidad, 0);
            dictionary_put(contabilidadEspecies, nombreEspecie, contabilidadEspecie);
        }
    }

    void agregarPokemonesCapturados(char * nombreEspecie, void * cantidad) {
        if (dictionary_has_key(contabilidadEspecies, nombreEspecie)) {
            ContabilidadEspecie * contabilidadEspecie = dictionary_get(contabilidadEspecies, nombreEspecie);
            contabilidadEspecie->capturados += (int) cantidad;
        } else {
            ContabilidadEspecie * contabilidadEspecie = crearContabilidadEspecie(0, (int) cantidad);
            dictionary_put(contabilidadEspecies, nombreEspecie, (void *) contabilidadEspecie);
        }
    }

    for(int i = 0; i < list_size(entrenadores); i++) {
        Entrenador * entrenador = list_get(entrenadores, i);
        dictionary_iterator(entrenador->pokemonesObjetivo, agregarPokemonesNecesarios);
        dictionary_iterator(entrenador->pokemonesCapturados, agregarPokemonesCapturados);
    }

    return contabilidadEspecies;
}

ContabilidadEspecie * crearContabilidadEspecie(int necesarios, int capturados) {
    ContabilidadEspecie * contabilidadEspecie = malloc(sizeof(ContabilidadEspecie));
    contabilidadEspecie->necesarios = necesarios;
    contabilidadEspecie->capturados = capturados;
    return contabilidadEspecie;
}