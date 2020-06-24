//
// Created by Alan Zhao on 30/05/2020.
//

#ifndef TEAM_MAPA_H
#define TEAM_MAPA_H

#include "app/Global.h"
#include "delibird/utils/random/Random.h"

/**
 * Aca llevamos cuenta de la posicion de los entes del tp que utilicen coordenadas.
 * Hasta donde sabemos, tanto entrenadores como pokemones tienen una posicion (x,y) asignada.
 * Cumple la funcion de ser el lugar para depositar pokemones xd.
 * El mapa es responsable de setearle el (x,y) a las cosas despues de moverlas en su memoria.
 * TODO: Definir que estructura usar. Por ahi necesito 2 dicts ya que no tengo forma de decir que hay en una posicion
 * Entrenador -> Registra en mapa <- HASH
 * Cliente del mapa -> (x,y) -> Consulta dict <- Si no hay nada NULL, sino Presencia(hash_id, tipo)
 * Cliente del mapa -> HASH -> Iterar dict <- KeyValue[(x,y), Presencia], devolvemos la presencia.
 */

typedef t_dictionary * Plano;
typedef t_list * Casilla;

typedef enum TipoPosicionable {
    ENTRENADOR,
    POKEMON
} TipoPosicionable;

typedef struct Presencia {
    char * uuid;
    TipoPosicionable tipoPosicionable;
} Presencia;

typedef struct Mapa {
    t_log * logger;
    Plano plano;
    // Interfaz publica
    // hayPokemonesAtrapables() : bool - Es true si hay pokemones para atrapar.
    char * (*registrarPosicion)(struct Mapa * this, Coordinate posicion, TipoPosicionable tipoPosicionable);
    void (*destruir)(struct Mapa * this);
} Mapa;

extern const struct MapaClass {
    Mapa (*new)();
} MapaConstructor;

// Funciones estaticas
Presencia * crearPresencia(TipoPosicionable posicionable);
Plano crearPlano();
Casilla crearCasilla();
void destruirCasilla(Casilla casilla);
void destruirPresencia(Presencia * presencia);
char * coordenadaImprimible(Coordinate posicion);
char * nombreTipoPosicionable(TipoPosicionable posicionable);

Mapa mapa;

#endif //TEAM_MAPA_H
