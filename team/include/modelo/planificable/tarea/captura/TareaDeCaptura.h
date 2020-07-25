//
// Created by Alan Zhao on 07/07/2020.
//

#ifndef TEAM_TAREADECAPTURA_H
#define TEAM_TAREADECAPTURA_H

#include "modelo/planificable/tarea/TareaPlanificable.h"
#include "modelo/pokemon/CapturaPokemon.h"
#include "modelo/pokemon/PokemonAtrapable.h"
#include "cliente/ClienteBrokerV2.h"
#include "manejadorDeEventos/registro/RegistradorDeEventos.h"
#include "servicios/servicioDeCaptura/ServicioDeCaptura.h"

TareaPlanificable * generarTareaDeCaptura(Entrenador * entrenador, char * pokemon, Coordinate posicionPokemon);

void iniciarCaptura(Entrenador * entrenador,char * especie, Coordinate * posicionPokemon);

#endif //TEAM_TAREADECAPTURA_H
