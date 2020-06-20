//
// Created by Alan Zhao on 13/06/2020.
//

#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "planificador/algoritmos/fifo/FirstInFirstOut.h"

AlgoritmoPlanificador obtenerAlgoritmo(char * nombreAlgoritmo) {
    if (string_equals_ignore_case(nombreAlgoritmo, "FIFO")) {
        return firstInFirstOut;
    } else {
        return firstInFirstOut;
    }
}

void inicializarAlgoritmosDePlanificacion() {
    firstInFirstOut = FirstInFirstOutConstructor.new();
}