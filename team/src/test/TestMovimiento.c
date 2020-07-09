//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeMovimiento() {
    log_info(testLogger, "Testeando la correcta generacion de caminos entre coordenadas");
    Coordinate origen = {.pos_x = 1, .pos_y = 3};
    Coordinate destino = {.pos_x = 5, .pos_y = 2};

    Camino camino = generarCamino(origen, destino);

    assert(list_size(camino) == 5); // [(2,3), (3,3), (4,3), (5,3), (5,2)]

    Coordinate * paso1 = list_get(camino, 0);
    Coordinate * paso2 = list_get(camino, 1);
    Coordinate * paso3 = list_get(camino, 2);
    Coordinate * paso4 = list_get(camino, 3);
    Coordinate * paso5 = list_get(camino, 4);

    assert(paso1->pos_x == 2 && paso1->pos_y == 3);
    assert(paso2->pos_x == 3 && paso2->pos_y == 3);
    assert(paso3->pos_x == 4 && paso3->pos_y == 3);
    assert(paso4->pos_x == 5 && paso4->pos_y == 3);
    assert(paso5->pos_x == 5 && paso5->pos_y == 2);

    list_destroy_and_destroy_elements(camino, free);
}
