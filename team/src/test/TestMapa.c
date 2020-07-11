//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeMapa() {
    log_info(testLogger, "Testeando la creacion del mapa");
    Mapa mapita = MapaConstructor.new();
    log_info(testLogger, "Testeando el registro de posicionables en el mapa");
    Entrenador * entrenadorARegistrar = EntrenadorConstructor.new("3|3", "A", "B");

    char * uuidRegistro = registrarEnMapaPosicionEntrenador(&mapita, entrenadorARegistrar);
    Posicion posicionDelEntrenadorRegistrado = entrenadorARegistrar->posicion(entrenadorARegistrar);

    assert(string_equals(uuidRegistro, entrenadorARegistrar->gps->uuid));
    assert(posicionDelEntrenadorRegistrado.valida == true);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_x == 3);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_y == 3);

    log_info(testLogger, "Testeando el movimiento de un entrenador");
    Coordinate * destino = create_coordinate(3,4);
    entrenadorARegistrar->mover(entrenadorARegistrar, destino);
    posicionDelEntrenadorRegistrado = entrenadorARegistrar->posicion(entrenadorARegistrar);

    assert(posicionDelEntrenadorRegistrado.valida == true);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_x == 3);
    assert(posicionDelEntrenadorRegistrado.coordenada.pos_y == 4);

    free(destino);
    entrenadorARegistrar->destruir(entrenadorARegistrar);
    mapita.destruir(&mapita);
}
