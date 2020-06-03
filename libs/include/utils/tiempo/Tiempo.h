//
// Created by Alan Zhao on 29/04/2020.
//

#ifndef SUPPORT_TIEMPO_H
#define SUPPORT_TIEMPO_H

#include <time.h>
#include <sys/time.h>

time_t ahoraEnTimeT();

double diferenciaTimevalsEnMillis(struct timeval tiempoFinal, struct timeval tiempoInicio);

struct timeval ahoraEnTimeval();

double milisegundosHastaAhora(struct timeval tiempoInicio);

double milisegundos(struct timeval tiempo);

#endif //SUPPORT_TIEMPO_H
