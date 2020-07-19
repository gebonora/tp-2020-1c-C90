//
// Created by Alan Zhao on 13/06/2020.
//

#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "planificador/algoritmos/fifo/FirstInFirstOut.h"
#include "planificador/algoritmos/sjf/ShortestJobFirst.h"
#include "planificador/algoritmos/rr/RoundRobin.h"


AlgoritmoPlanificador obtenerAlgoritmo(char * nombreAlgoritmo) {
    if (string_equals_ignore_case(nombreAlgoritmo, "FIFO")) {
        return firstInFirstOut;
    } else if (string_equals_ignore_case(nombreAlgoritmo, "RR")){
        return roundRobin;
    } else if (string_equals_ignore_case(nombreAlgoritmo, "SJF-CD")) {
        return shortestRemainingTimeFirst;
    } else if (string_equals_ignore_case(nombreAlgoritmo, "SJF-SD")) {
        return shortestJobFirst;
    } else {
        return firstInFirstOut;
    }
}

void inicializarAlgoritmosDePlanificacion() {
    firstInFirstOut = FirstInFirstOutConstructor.new();
    shortestJobFirst = ShortestJobFirstConstructor.new();
    roundRobin = RoundRobinConstructor.new();
    shortestRemainingTimeFirst = ShortestJobFirstConstructor.new();
}

void destruirAlgoritmosDePlanificacion(){
	firstInFirstOut.destruir(&firstInFirstOut);
	shortestJobFirst.destruir(&shortestJobFirst);
	roundRobin.destruir(&roundRobin);
	shortestRemainingTimeFirst.destruir(&shortestRemainingTimeFirst);
}

char * nombreAlgoritmoCompleto(char * nombreCorto) {
    if (string_equals_ignore_case(nombreCorto, "FIFO")) {
        return "FIFO - First In First Out";
    } else if (string_equals_ignore_case(nombreCorto, "RR")) {
        return "RR - Round Robin";
    } else if (string_equals_ignore_case(nombreCorto, "SJF-CD")) {
        return "SJF-CD - Shortest Job First CON desalojo";
    } else if (string_equals_ignore_case(nombreCorto, "SJF-SD")) {
        return "SJF-SD - Shortest Job First SIN desalojo";
    } else {
        return "Desconocido - Asumimos FIFO por defecto";
    }
}
