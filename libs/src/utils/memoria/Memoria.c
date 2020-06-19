//
// Created by Alan Zhao on 18/06/2020.
//

#include "utils/memoria/Memoria.h"

void liberarArrayDePunterosContados(void ** arrayDePunteros, int size) {
    for (int i = 0; i < size; i++) {
        free(arrayDePunteros[i]);
    }
    free(arrayDePunteros);
}

void liberarArrayDePunteros(void ** arrayDePunteros) {
    int cantidad = cantidadPunterosEnArray(arrayDePunteros);
    liberarArrayDePunterosContados(arrayDePunteros, cantidad);
}

int cantidadPunterosEnArray(void ** arrayDePunteros) {
    int cantidad = 0;
    void ** comienzo = arrayDePunteros;
    while(*arrayDePunteros != NULL) {
        arrayDePunteros++;
        cantidad++;
    }
    arrayDePunteros = comienzo; //Para recuperar el estado inicial del puntero al array
    return cantidad;
}