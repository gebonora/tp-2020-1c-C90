#ifndef TEAM_SHORTESTREMAININGTIMEFIRST_H
#define TEAM_SHORTESTREMAININGTIMEFIRST_H

#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "modelo/planificable/HiloEntrenadorPlanificable.h"

double alpha_SRTF;
double estInicial_SRTF;

extern const struct ShortestRemainingTimeFirstClass {
	AlgoritmoPlanificador (*new)();
} ShortestRemainingTimeFirstConstructor;

void *ObtenerProximoMasCortoSRTF(t_list *lista);
bool unidadConRafagaEstimadaMasCortaSRTF(void* elem1, void* elem2);
double calcularEstimacionSRTF(UnidadPlanificable *elem);
void asignarEstimacionAUnElementoSRTF(void* elem);
void asignarEstimacionATodosLosElementosDeLaListaSRTF(t_list *list);
void obtenerConfiguracionesSRTF();

#endif //TEAM_SHORTESTREMAININGTIMEFIRST_H
