#include "planificador/algoritmos/sjf/ShortestJobFirst.h"

static UnidadPlanificable * proximoAEjecutar(AlgoritmoPlanificador * this, t_list * listaReady) {
    if (list_is_empty(listaReady)) {
        log_error(this->logger, "No hay nada para planificar.");
        return NULL;
    } else {
		obtenerConfiguracionesSJF();
        UnidadPlanificable * unindadProximaAEjecutar = list_get(listaReady, 0);
        log_debug(this->logger, "Proxima unidad planificable a ejecutar seleccionada con éxito");
        return unindadProximaAEjecutar;
    }
}


void *ObtenerProximoMasCorto(t_list *lista){

	// aqui utilizaria list_sort para ordenar la lista por la rafaga est actual
	// mas corta y luego proximoAEjecutar simplemente tomaria el mas corto de esta.

	return 0;

}

double calcularEstimacion(UnidadPlanificable *elem){
	double est_ant = elem->est_raf_ant;
	double real_ant = elem->real_raf_ant;

	return ((1-alpha)*est_ant) + (alpha * real_ant);
}

void asignarEstimacionAUnElemento(UnidadPlanificable *elem){
	elem->est_raf_ant = elem->est_raf_actual; // asignamos la actual anterior como est anterior

	elem->est_raf_actual = calcularEstimacion(elem); // calculamos el nuevo estimado

}
// faltaria ver donde obtenemos y actualizamos la rafaga real anterior

void asignarEstimacionATodosLosElementosDeLaLista(t_list *list){
	list_map(list,(void*) asignarEstimacionAUnElemento()); // duda sobre como funciona esta funcion
}

void obtenerConfiguracionesSJF(){
	t_config* configSJF = config_create(TEAM_CONFIG_FILE);

	alpha = config_get_double_value(configSJF, "ALPHA");
	estInicial = config_get_double_value(configSJF, "ESTIMACION_INICIAL");

	config_destroy(configSJF);

}


