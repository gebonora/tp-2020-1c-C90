#ifndef TEAM_SHORTESTJOBFIRST_H
#define TEAM_SHORTESTJOBFIRST_H

#include "planificador/algoritmos/AlgoritmoPlanificador.h"
#include "modelo/planificable/HiloEntrenadorPlanificable.h"

double alpha;
double estInicial;

extern const struct ShortestJobFirstClass {
	AlgoritmoPlanificador (*new)();
} ShortestJobFirstConstructor;

void *ObtenerProximoMasCorto(t_list *lista);
bool unidadConRafagaEstimadaMasCorta(void* elem1, void* elem2);
double calcularEstimacion(UnidadPlanificable *elem);
void* asignarEstimacionAUnElemento(void* elem);
void asignarEstimacionATodosLosElementosDeLaLista(t_list *list);
void obtenerConfiguracionesSJF();

#endif //TEAM_SHORTESTJOBFIRST_H
