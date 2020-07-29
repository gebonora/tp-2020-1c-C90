//
// Created by Alan Zhao on 26/06/2020.
//

#include "modelo/objetivo/ObjetivoGlobal.h"

void restarUnCapturado(ObjetivoGlobal * this, char * especie) {
    if (dictionary_has_key(this->contabilidadEspeciesActualizable, especie)) {
        pthread_mutex_lock(&this->mutexContabilidadActualizable);
        int * capturados = dictionary_get(this->contabilidadEspeciesActualizable, especie);
        (*capturados)--;
        pthread_mutex_unlock(&this->mutexContabilidadActualizable);
    }
}

void sumarUnCapturado(ObjetivoGlobal * this, char * especie) {
    if (dictionary_has_key(this->contabilidadEspeciesActualizable, especie)) {
        pthread_mutex_lock(&this->mutexContabilidadActualizable);
        int * capturados = dictionary_get(this->contabilidadEspeciesActualizable, especie);
        (*capturados)++;
        pthread_mutex_unlock(&this->mutexContabilidadActualizable);
    }
}

t_list * especiesNecesarias(ObjetivoGlobal * this) {
    pthread_mutex_lock(&this->mutexContabilidadActualizable);
    t_list * especies = (t_list *) dictionary_keys(this->contabilidadEspeciesActualizable);
    pthread_mutex_unlock(&this->mutexContabilidadActualizable);
    bool esCapturable(void * nombrePokemon) {
        return this->puedeCapturarse(this, (char *) nombrePokemon);
    }
    t_list * especiesNecesarias = (t_list *) list_filter(especies, esCapturable);
    list_destroy(especies);
    return especiesNecesarias;
}

bool puedeCapturarse(ObjetivoGlobal * this, char * especiePokemon) {
    if (dictionary_has_key(this->contabilidadEspeciesActualizable, especiePokemon)) {
        pthread_mutex_lock(&this->mutexContabilidadActualizable);
        ContabilidadEspecie * contabilidadEspecie = dictionary_get(this->contabilidadEspeciesActualizable, especiePokemon);
        bool sePuede = contabilidadEspecie->necesarios > contabilidadEspecie->capturados;
        pthread_mutex_unlock(&this->mutexContabilidadActualizable);
        if (!sePuede) {
            log_debug(this->logger, "No se puede capturar a %s porque tenemos %d/%d", especiePokemon, contabilidadEspecie->capturados, contabilidadEspecie->necesarios);
        }
        return sePuede;
    }
    log_debug(this->logger, "No se puede capturar a %s por que no forma parte del universo de este proceso", especiePokemon);
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
    dictionary_iterator(this->contabilidadEspeciesInicial, agregarEntradaAString);
    log_debug(this->logger, "------------------------------------------");
}

void solicitarPokemones(ObjetivoGlobal * this) {
    t_list * especiesNecesarias = this->especiesNecesarias(this);

    for (int i = 0; i < list_size(especiesNecesarias); i++) {
        char * pokemon = list_get(especiesNecesarias, i);
        log_debug(this->logger, "Solicitando especie: %s", pokemon);
        RespuestaBroker respuestaBroker = this->clienteBroker->enviarGet(this->clienteBroker, pokemon);
        if (respuestaBroker.esValida) {
            this->registradorDeEventos->registrarGetEnEspera(this->registradorDeEventos, pokemon, respuestaBroker.idCorrelatividad);
        }
    }

    list_destroy(especiesNecesarias);
}

void destruirObjetivoGlobal(ObjetivoGlobal * this) {
    log_debug(this->logger, "Se procede a destruir el objetivo global");
    log_destroy(this->logger);
    dictionary_destroy_and_destroy_elements(this->contabilidadEspeciesInicial, free);
    dictionary_destroy_and_destroy_elements(this->contabilidadEspeciesActualizable, free);
    pthread_mutex_destroy(&this->mutexContabilidadActualizable);
}

static ObjetivoGlobal new(Equipo unEquipo, ClienteBrokerV2 * clienteBroker, RegistradorDeEventos * registradorDeEventos) {
    pthread_mutex_t mutexContabilidadActualizable;
    pthread_mutex_init(&mutexContabilidadActualizable, NULL);

    ObjetivoGlobal objetivo = {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "ObjetivoGlobal", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
            .contabilidadEspeciesInicial = calcularObjetivoEspecies(unEquipo),
            .contabilidadEspeciesActualizable = calcularObjetivoEspecies(unEquipo),
			.clienteBroker = clienteBroker,
			.registradorDeEventos = registradorDeEventos,
			.mutexContabilidadActualizable = mutexContabilidadActualizable,
            &especiesNecesarias,
            &puedeCapturarse,
            &imprimirObjetivoGlobal,
            &solicitarPokemones,
            &destruirObjetivoGlobal,
            //&especiesPokemonNecesarias,
            &sumarUnCapturado,
            &restarUnCapturado,
    };
    objetivo.imprimirObjetivoGlobal(&objetivo);
    return objetivo;
}

const struct ObjetivoGlobalClass ObjetivoGlobalConstructor = {.new=&new};

//Deprecado
t_list* especiesPokemonNecesarias(ObjetivoGlobal * this,t_dictionary* diccionarioPokemon){
	t_list* especiesPokemon;

	void agregarPokemonALista(char * nombreEspecie, void * cantidad){
		if(*(int*) (dictionary_get(diccionarioPokemon,nombreEspecie))  > 0){
			list_add(especiesPokemon, nombreEspecie);
		}
	}

	dictionary_iterator(diccionarioPokemon,agregarPokemonALista);

	return especiesPokemon;
}


t_dictionary * calcularObjetivoEspecies(Equipo entrenadores) {
    t_dictionary * contabilidadEspecies = dictionary_create();

    void agregarPokemonesNecesarios(char * nombreEspecie, void * cantidad) {
        if (dictionary_has_key(contabilidadEspecies, nombreEspecie)) {
            ContabilidadEspecie * contabilidadEspecie = dictionary_get(contabilidadEspecies, nombreEspecie);
            contabilidadEspecie->necesarios += *(int *) cantidad;
        } else {
            ContabilidadEspecie * contabilidadEspecie = crearContabilidadEspecie(*(int *) cantidad, 0);
            dictionary_put(contabilidadEspecies, nombreEspecie, contabilidadEspecie);
        }
    }

    void agregarPokemonesCapturados(char * nombreEspecie, void * cantidad) {
        if (dictionary_has_key(contabilidadEspecies, nombreEspecie)) {
            ContabilidadEspecie * contabilidadEspecie = dictionary_get(contabilidadEspecies, nombreEspecie);
            contabilidadEspecie->capturados += *(int *) cantidad;
        } else {
            ContabilidadEspecie * contabilidadEspecie = crearContabilidadEspecie(0, *(int *) cantidad);
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
