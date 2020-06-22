//
// Created by Alan Zhao on 21/06/2020.
//

#include "modelo/mapa/registro/RegistradorDePosiciones.h"

#define defineRegistrarEnMapaPosicion(T) \
char * registrarEnMapaPosicion##T(Mapa * mapaDeRegistro, T* posicionable){ \
    char * uuid = mapaDeRegistro->registrarPosicion(mapaDeRegistro, posicionable->posicionInicial, posicionable->tipoPosicionable); \
    posicionable->uuid = uuid;\
    return uuid; \
}

defineRegistrarEnMapaPosicion(Entrenador)