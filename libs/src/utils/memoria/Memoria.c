//
// Created by Alan Zhao on 18/06/2020.
//

#include "utils/memoria/Memoria.h"

void liberarArrayContado(void ** array, int size) {
    for (int i = 0; i < size; i++) {
        free(array[i]);
    }
    free(array);
}

void liberarArray(void ** array) {
    int cantidad = cantidadElementosEnArray(array);
    liberarArrayContado(array, cantidad);
}

int cantidadElementosEnArray(void ** array) {
    int cantidad = 0;
    void ** comienzo = array;
    while(*comienzo != NULL) {
        comienzo++;
        cantidad++;
    }
    return cantidad;
}