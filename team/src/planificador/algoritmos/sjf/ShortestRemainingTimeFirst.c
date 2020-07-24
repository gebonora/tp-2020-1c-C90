#include "planificador/algoritmos/sjf/ShortestRemainingTimeFirst.h"


static UnidadPlanificable * proximoAEjecutar(AlgoritmoPlanificador * this, t_list * listaReady) {
	if (list_is_empty(listaReady)) {
		log_error(this->logger, "No hay nada para planificar.");
		return NULL;
	} else {
		UnidadPlanificable * unindadProximaAEjecutar = ObtenerProximoMasCortoSRTF(listaReady);
		log_debug(this->logger, "Proxima unidad planificable a ejecutar seleccionada con éxito");
		return unindadProximaAEjecutar;
	}
}

void *ObtenerProximoMasCortoSRTF(t_list *lista) {

	// aqui utilizaria list_sort para ordenar la lista por la rafaga est actual
	// mas corta y luego proximoAEjecutar simplemente tomaria el mas corto de esta.
	list_sort(lista, unidadConRafagaEstimadaMasCortaSRTF);
	return list_get(lista, 0);

}

bool unidadConRafagaEstimadaMasCortaSRTF(void* elem1, void* elem2) {

	UnidadPlanificable* unidad1 = (UnidadPlanificable*) elem1;
	UnidadPlanificable* unidad2 = (UnidadPlanificable*) elem2;


	return (unidad1->infoUltimaEjecucion.est_raf_actual
			- unidad1->infoUltimaEjecucion.rafaga_parcial_ejecutada)
			<= (unidad2->infoUltimaEjecucion.est_raf_actual
			- unidad2->infoUltimaEjecucion.rafaga_parcial_ejecutada);

	// esta función retorna true si la rafaga estimada de elem1 es menos o igual a la de elem2
}

double calcularEstimacionSRTF(UnidadPlanificable *elem) {
	double estimacion;
	if (elem->infoUltimaEjecucion.seEjecutoPrimeraEstimacion == false) {
		estimacion = estInicial_SRTF;
		elem->infoUltimaEjecucion.seEjecutoPrimeraEstimacion = true;
	} else {

		double est_ant = elem->infoUltimaEjecucion.est_raf_ant;
		double real_ant = elem->infoUltimaEjecucion.real_raf_ant;
		estimacion = ((1 - alpha_SRTF) * est_ant) + (alpha_SRTF * real_ant);
	}
	return estimacion;
}


void asignarEstimacionAUnElementoSRTF(void* elem) {

	UnidadPlanificable* unidad = (UnidadPlanificable*) elem;

	if(unidad->infoUltimaEjecucion.rafaga_real_actual <= unidad->infoUltimaEjecucion.rafaga_parcial_ejecutada){
	unidad->infoUltimaEjecucion.est_raf_ant = unidad->infoUltimaEjecucion.est_raf_actual;
	// asignamos la actual anterior como est anterior
	unidad->infoUltimaEjecucion.est_raf_actual = calcularEstimacionSRTF(unidad);
	// calculamos el nuevo estimado y se lo asignamos
	unidad->infoUltimaEjecucion.rafaga_parcial_ejecutada = 0;
	}
}

void asignarEstimacionATodosLosElementosDeLaListaSRTF(t_list *list) {
	list_iterate(list, asignarEstimacionAUnElementoSRTF);
}

void obtenerConfiguracionesSRTF() {
	t_config* configSRTF = config_create(TEAM_CONFIG_FILE);

	alpha_SRTF = config_get_double_value(configSRTF, "ALPHA");
	estInicial_SRTF = config_get_double_value(configSRTF, "ESTIMACION_INICIAL");

	config_destroy(configSRTF);
}

static void destruir(AlgoritmoPlanificador * this) {
	log_destroy(this->logger);
}

static AlgoritmoPlanificador new() {
	obtenerConfiguracionesSRTF();
	return (AlgoritmoPlanificador ) {
		.logger = log_create(TEAM_INTERNAL_LOG_FILE, "AlgoritmoSJFconDesalojo", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
		.tipo = SJF_CD,
		&proximoAEjecutar,
		&destruir
	} ;
}

const struct ShortestRemainingTimeFirstClass ShortestRemainingTimeFirstConstructor = {.new=&new};

