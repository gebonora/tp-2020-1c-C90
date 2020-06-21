//
// Created by Alan Zhao on 29/04/2020.
//

#ifndef DELIBIRD_TIEMPO_H
#define DELIBIRD_TIEMPO_H

#include <time.h>
#include <sys/time.h>

time_t ahoraEnTimeT();

double diferenciaTimevalsEnMillis(struct timeval tiempoFinal, struct timeval tiempoInicio);

struct timeval ahoraEnTimeval();

double milisegundosHastaAhora(struct timeval tiempoInicio);

double milisegundos(struct timeval tiempo);

char* current_date_time_as_string();

#endif //DELIBIRD_TIEMPO_H
