//
// Created by Alan Zhao on 27/05/2020.
//

#ifndef TEAM_ENTRENADOR_H
#define TEAM_ENTRENADOR_H


#include "app/Global.h"
#include "delibird/servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"
#include "delibird/utils/memoria/Memoria.h"
#include "delibird/utils/colecciones/ExtensionColecciones.h"
#include "modelo/mapa/gps/Gps.h"
#include "modelo/mapa/coordenadas/UtilidadesCoordenadas.h"
#include "modelo/mapa/movimiento/Movimiento.h"

/**
 * El entrenador se mueve y caza pokemones. La idea es llevar ese registro acá.
 */

typedef t_dictionary * ContadorPokemones; // La key es el nombre del pokemon, el value es la cantidad.

typedef struct Entrenador {
    t_log * logger;
    char * id;
    Gps * gps;
    TipoPosicionable tipoPosicionable;
    Coordinate posicionInicial;
    ContadorPokemones pokemonesCapturados;
    ContadorPokemones pokemonesObjetivo;
    int limiteDeCaptura; // Establece cuantos pokemones puede capturar
    // Interfaz publica
    void (*mover)(struct Entrenador * this, Coordinate * posicionObjetivo);
    bool (*objetivoCompletado)(struct Entrenador * this); // Es true si pokemones_objetivo es igual a pokemones_capturados.
    bool (*puedeAtraparPokemones)(struct Entrenador * this); // Es true si es < a limiteDeCaptura.
    Posicion (*posicion)(struct Entrenador * this); // Le pregunta al mapa donde esta, pasandole su uuid.
    // TODO: void (*agregarPokemonCapturado)(struct Entrenador * this, char * especiePokemon);
    void (*destruir)(struct Entrenador * this);
} Entrenador;

extern const struct EntrenadorClass {
    Entrenador *(*new)(char * posicionInicial, char * pokemones_iniciales, char * pokemones_objetivos);
} EntrenadorConstructor;

// Funciones estaticas
ContadorPokemones agruparPokemonesPorNombre(char * nombreDeLosPokemones);
int totalDePokemones(ContadorPokemones contadorPokemones); // Nos dice cuantos hay en el contador
int contadorEntrenadores;

#endif //TEAM_ENTRENADOR_H
