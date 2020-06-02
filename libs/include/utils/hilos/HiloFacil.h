//
// Created by Alan Zhao on 29/04/2020.
//

#ifndef SUPPORT_HILOFACIL_H
#define SUPPORT_HILOFACIL_H

#include <pthread.h>

int crearHilo(void *funcion(void *), void *parametros);

#endif //SUPPORT_HILOFACIL_H
