//
// Created by Alan Zhao on 29/04/2020.
//

#include "utils/hilos/HiloFacil.h"

int crearHilo(void *funcion(void *), void *parametros) {
    pthread_attr_t attr;
    pthread_t hilo;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int resultado = pthread_create(&hilo, &attr, funcion, parametros);

    pthread_attr_destroy(&attr);

    return resultado;
}
