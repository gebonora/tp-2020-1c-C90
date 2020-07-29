//
// Created by Alan Zhao on 30/05/2020.
//

#ifndef TEAM_OBJETIVOGLOBAL_H
#define TEAM_OBJETIVOGLOBAL_H

#include "semaphore.h"
#include "app/Global.h"
#include "modelo/equipo/Equipo.h"
#include "delibird/utils/colecciones/ExtensionColecciones.h"
#include "cliente/ClienteBrokerV2.h"
#include "manejadorDeEventos/registro/RegistradorDeEventos.h"

/**
 * Necesario para tomar decisiones de captura camino a completar la finalidad del team.
 */

typedef struct ContabilidadEspecie {
    int necesarios;
    int capturados;
} ContabilidadEspecie;

typedef struct ObjetivoGlobal {
    t_log *logger;
    t_dictionary * contabilidadEspeciesInicial; // La key es el nombre del pokemon, el value seria CotabilidadEspecie
    t_dictionary * contabilidadEspeciesActualizable;
    ClienteBrokerV2 * clienteBroker;
    RegistradorDeEventos * registradorDeEventos;
    pthread_mutex_t mutexContabilidadActualizable;
    //Interfaz publica
    t_list * (*especiesNecesarias)(struct ObjetivoGlobal * this); //Nos dice los unique de los pokemones necesarios. (ver GET)
    bool (*puedeCapturarse)(struct ObjetivoGlobal * this, char * especiePokemon); // Nos dice si todavia necesitamos el pokemon para cumplir el objetivo. (ver LOCALIZED)
    void (*imprimirObjetivoGlobal)(struct ObjetivoGlobal * this);
    void (*solicitarUbicacionPokemonesNecesitados)(struct ObjetivoGlobal * this);
    void (*destruirObjetivoGlobal)(struct ObjetivoGlobal * this);
    //t_list* (*especiesPokemonNecesarias)(struct ObjetivoGlobal * this, t_dictionary* diccionarioPokemon);
    void (*sumarUnCapturado)(struct ObjetivoGlobal * this, char * especie);
    void (*restarUnCapturado)(struct ObjetivoGlobal * this, char * especie);
} ObjetivoGlobal;

extern const struct ObjetivoGlobalClass {
    ObjetivoGlobal (*new)(Equipo equipo, ClienteBrokerV2 * clienteBroker, RegistradorDeEventos * registradorDeEventos);
} ObjetivoGlobalConstructor;

//Funciones estaticas
t_dictionary * calcularObjetivoEspecies(Equipo equipo);
ContabilidadEspecie * crearContabilidadEspecie(int necesarios, int capturados);

ObjetivoGlobal objetivoGlobalProcesoTeam;


#endif //TEAM_OBJETIVOGLOBAL_H
