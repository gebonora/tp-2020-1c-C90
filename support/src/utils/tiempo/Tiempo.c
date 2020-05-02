//
// Created by Alan Zhao on 29/04/2020.
//

#include "utils/tiempo/Tiempo.h"

time_t ahoraEnTimeT(){
    return time(NULL);
}

struct timeval ahoraEnTimeval() {
    struct timeval tiempoFinal;
    gettimeofday(&tiempoFinal, NULL);
    return tiempoFinal;
}

double diferenciaTimevalsEnMillis(struct timeval tiempoFinal, struct timeval tiempoInicio) {
    return milisegundos(tiempoFinal) - milisegundos(tiempoInicio);
}

double milisegundosHastaAhora(struct timeval tiempoInicio) {
    return milisegundos(ahoraEnTimeval()) - milisegundos(tiempoInicio);
}

double milisegundos(struct timeval tiempo) {
    return (tiempo.tv_sec * 1000) + (tiempo.tv_usec / 1000.0);
}