//
// Created by Alan Zhao on 30/05/2020.
//

#ifndef TEAM_OBJETIVOGLOBAL_H
#define TEAM_OBJETIVOGLOBAL_H

#include "app/Global.h"
#include "modelo/equipo/Equipo.h"
#include "delibird/utils/colecciones/ExtensionColecciones.h"

/**
 * Necesario para tomar decisiones de captura camino a completar la finalidad del team.
 */

typedef struct ContabilidadEspecie {
    int necesarios;
    int capturados;
} ContabilidadEspecie;

typedef struct ObjetivoGlobal {
    t_log *logger;
    t_dictionary * contabilidadEspecies; // La key es el nombre del poke,on, el value seria CotabilidadEspecie
    //Interfaz publica
    t_list * (*especiesNecesarias)(struct ObjetivoGlobal * this); //Nos dice los unique de los pokemones necesarios. (ver GET)
    bool (*puedeCapturarse)(struct ObjetivoGlobal * this, char * especiePokemon); // Nos dice si todavia necesitamos el pokemon para cumplir el objetivo. (ver LOCALIZED)
    void (*destruirObjetivoGlobal)(struct ObjetivoGlobal * this);
} ObjetivoGlobal;

extern const struct ObjetivoGlobalClass {
    ObjetivoGlobal (*new)(Equipo equipo);
} ObjetivoGlobalConstructor;

//Funciones estaticas
t_dictionary * calcularObjetivoEspecies(Equipo equipo);
ContabilidadEspecie * crearContabilidadEspecie(int necesarios, int capturados);

ObjetivoGlobal objetivoGlobal;

#endif //TEAM_OBJETIVOGLOBAL_H
