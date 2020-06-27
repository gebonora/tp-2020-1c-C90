//
// Created by Alan Zhao on 26/06/2020.
//

#include "modelo/objetivo/ObjetivoGlobal.h"

t_list * especiesNecesarias(ObjetivoGlobal * this) {
    return list_create();
}

bool puedeCapturarse(ObjetivoGlobal * this, char * especiePokemon) {
    return true;
}

void destruirObjetivoGlobal(ObjetivoGlobal * this) {
    log_destroy(this->logger);
    dictionary_destroy_and_destroy_elements(this->contabilidadEspecies, free);
}

static ObjetivoGlobal new(Equipo equipo) {
    return (ObjetivoGlobal) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "ObjetivoGlobal", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO),
            .contabilidadEspecies = calcularObjetivoEspecies(equipo),
            &especiesNecesarias,
            &puedeCapturarse,
            &destruirObjetivoGlobal
    };
}

const struct ObjetivoGlobalClass ObjetivoGlobalConstructor = {.new=&new};

t_dictionary * calcularObjetivoEspecies(Equipo entrenadores) {
    t_dictionary * contabilidadEspecies = dictionary_create();

    void agregarPokemonesNecesarios(char * nombreEspecie, void * cantidad) {
        if (dictionary_has_key(contabilidadEspecies, nombreEspecie)) {
            ContabilidadEspecie * contabilidadEspecie = dictionary_get(contabilidadEspecies, nombreEspecie);
            contabilidadEspecie->necesarios += cantidad;
        } else {
            ContabilidadEspecie * contabilidadEspecie = crearContabilidadEspecie(cantidad, 0);
            dictionary_put(contabilidadEspecies, nombreEspecie, contabilidadEspecie);
        }
    }

    void agregarPokemonesCapturados(char * nombreEspecie, void * cantidad) {
        if (dictionary_has_key(contabilidadEspecies, nombreEspecie)) {
            ContabilidadEspecie * contabilidadEspecie = dictionary_get(contabilidadEspecies, nombreEspecie);
            contabilidadEspecie->capturados += cantidad;
        } else {
            ContabilidadEspecie * contabilidadEspecie = crearContabilidadEspecie(0, cantidad);
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