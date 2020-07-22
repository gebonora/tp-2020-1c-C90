//
// Created by Alan Zhao on 15/07/2020.
//

#ifndef TEAM_SERVICIODECAPTURA_H
#define TEAM_SERVICIODECAPTURA_H

#include "app/Global.h"
#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"
#include "modelo/mapa/Mapa.h"
#include "modelo/equipo/Equipo.h"
#include "modelo/pokemon/PokemonAtrapable.h"
#include "modelo/pokemon/CapturaPokemon.h"
#include "modelo/mapa/coordenadas/UtilidadesCoordenadas.h"
#include "modelo/mapa/registro/RegistradorDePosiciones.h"

typedef struct ServicioDeCaptura {
    t_log * logger;
    Mapa mapa;
    ServicioDePlanificacion * servicioDePlanificacion;
    t_list * pokemonesAtrapables;
    //Interfaz publica
    bool (*registrarCapturaExitosa)(struct ServicioDeCaptura * this, CapturaPokemon * capturaPokemon);
    void (*procesarPokemonCapturable)(struct ServicioDeCaptura * this, char * especie, Coordinate posicion);
    //Metodos privados
    PokemonAtrapable * (*obtenerPokemonAtrapable)(struct ServicioDeCaptura * this, char * especie, Coordinate posicion);
    PokemonAtrapable * (*altaDePokemon)(struct ServicioDeCaptura * this, char * especie, Coordinate posicion);
    void (*encargarTrabajoDeCaptura)(struct ServicioDeCaptura * this, char * especie, Coordinate posicion);
    void (*destruir)(struct ServicioDeCaptura * this);
} ServicioDeCaptura;

extern const struct ServicioDeCapturaClass {
    ServicioDeCaptura * (*new)(Mapa mapa, ServicioDePlanificacion * servicioDePlanificacion);
} ServicioDeCapturaConstructor;

ServicioDeCaptura * servicioDeCapturaProcesoTeam;

void destruirPokemonAtrapable(PokemonAtrapable * pokemonAtrapable);

#endif //TEAM_SERVICIODECAPTURA_H
