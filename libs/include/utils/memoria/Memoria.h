//
// Created by Alan Zhao on 18/06/2020.
//

#ifndef DELIBIRD_MEMORIA_H
#define DELIBIRD_MEMORIA_H

#include <stdlib.h>

void liberarArrayDePunterosContados(void ** arrayDePunteros, int size);
void liberarArrayDePunteros(void ** arrayDePunteros);
int cantidadPunterosEnArray(void ** arrayDePunteros);

#endif //DELIBIRD_MEMORIA_H
