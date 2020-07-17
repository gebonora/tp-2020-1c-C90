#include "planificador/algoritmos/sjf/ShortestJobFirst.h"

//TODO: faltaria ver donde obtenemos y actualizamos la rafaga real anterior


static UnidadPlanificable * proximoAEjecutar(AlgoritmoPlanificador * this, t_list * listaReady) {
    if (list_is_empty(listaReady)) {
        log_error(this->logger, "No hay nada para planificar.");
        return NULL;
    } else {
    	if(primeraEjecucion == 0){
    		obtenerConfiguracionesSJF();
    		primeraEjecucion = 1;
    	}
        UnidadPlanificable * unindadProximaAEjecutar = ObtenerProximoMasCorto(listaReady);
        log_debug(this->logger, "Proxima unidad planificable a ejecutar seleccionada con éxito");
        return unindadProximaAEjecutar;
    }
}


void *ObtenerProximoMasCorto(t_list *lista){

	// aqui utilizaria list_sort para ordenar la lista por la rafaga est actual
	// mas corta y luego proximoAEjecutar simplemente tomaria el mas corto de esta.
	list_sort(lista,unidadConRafagaEstimadaMasCorta(lista->head->data, lista->head->next->data));
	return list_get(lista, 0);

}

bool unidadConRafagaEstimadaMasCorta(UnidadPlanificable *elem1, UnidadPlanificable *elem2){

	return ((HiloEntrenadorPlanificable*)elem1 ->unidad)->infoUltimaEjecucion->est_raf_actual
			<= ((HiloEntrenadorPlanificable*)elem2 ->unidad)->infoUltimaEjecucion->est_raf_actual;

	// esta función retorna true si la rafaga estimada de elem1 es menos o igual a la de elem2
}

double calcularEstimacion(UnidadPlanificable *elem){
	double estimacion;
	if(((HiloEntrenadorPlanificable*)elem ->unidad)->infoUltimaEjecucion->primeraEstimacion == false){
	estimacion = estInicial;
	((HiloEntrenadorPlanificable*)elem ->unidad)->infoUltimaEjecucion->primeraEstimacion = true;
	}else{
	double est_ant = ((HiloEntrenadorPlanificable*)elem ->unidad)->infoUltimaEjecucion->est_raf_ant;
	double real_ant = ((HiloEntrenadorPlanificable*)elem ->unidad)->infoUltimaEjecucion->real_raf_ant;

	estimacion = ((1-alpha)*est_ant) + (alpha * real_ant);
	}

	return estimacion;
}

void asignarEstimacionAUnElemento(UnidadPlanificable *elem){

	((HiloEntrenadorPlanificable*)elem ->unidad)->infoUltimaEjecucion->est_raf_ant
			= ((HiloEntrenadorPlanificable*)elem ->unidad)->infoUltimaEjecucion->est_raf_actual;
	 // asignamos la actual anterior como est anterior

	((HiloEntrenadorPlanificable*)elem ->unidad)->infoUltimaEjecucion->est_raf_actual
			= calcularEstimacion(elem);
	// calculamos el nuevo estimado y se lo asignamos

}

void asignarEstimacionATodosLosElementosDeLaLista(t_list *list){
	list_map(list, asignarEstimacionAUnElemento(list->head->data)); // duda si esta pasado bien el parametro
}


void obtenerConfiguracionesSJF(){
	t_config* configSJF = config_create(TEAM_CONFIG_FILE);

	alpha = config_get_double_value(configSJF, "ALPHA");
	estInicial = config_get_double_value(configSJF, "ESTIMACION_INICIAL");

	config_destroy(configSJF);
}


