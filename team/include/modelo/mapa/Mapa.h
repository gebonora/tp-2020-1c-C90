//
// Created by Alan Zhao on 30/05/2020.
//

#ifndef TEAM_MAPA_H
#define TEAM_MAPA_H

/**
 * Confirmar si es necesario. Aca llevamos cuenta de la posicion de los entes del tp que utilicen coordenadas.
 * Hasta donde sabemos, tanto entrenadores como pokemones tienen una posicion (x,y) asignada.
 * Puede que no sea necesario, ya que todos los entes ubicables tienen un atributo coordenada.
 * Actually, necesito un lugar para depositar pokemones xd.
 * Puedo hacer que el mapa sea responsable de setearle el (x,y) a las cosas despues de moverlas en su memoria.
 */

typedef struct Mapa {
    // Interfaz publica
    // hayPokemonesAtrapables() : bool - Es true si hay pokemones para atrapar.
} Mapa;

#endif //TEAM_MAPA_H
