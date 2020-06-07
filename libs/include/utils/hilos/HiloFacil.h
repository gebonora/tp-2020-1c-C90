//
// Created by Alan Zhao on 29/04/2020.
//

#ifndef DELIBIRD_HILOFACIL_H
#define DELIBIRD_HILOFACIL_H

#include <pthread.h>

int crearHilo(void *funcion(void *), void *parametros);

#endif //DELIBIRD_HILOFACIL_H
