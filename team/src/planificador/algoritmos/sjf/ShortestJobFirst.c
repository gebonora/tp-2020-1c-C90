#include "planificador/algoritmos/sjf/ShortestJobFirst.h"

//TODO: faltaria ver donde obtenemos y actualizamos la rafaga real anterior

static UnidadPlanificable * proximoAEjecutar(AlgoritmoPlanificador * this, t_list * listaReady) {
	if (list_is_empty(listaReady)) {
		log_error(this->logger, "No hay nada para planificar.");
		return NULL;
	} else {
		UnidadPlanificable * unindadProximaAEjecutar = ObtenerProximoMasCorto(listaReady);
		log_debug(this->logger, "Proxima unidad planificable a ejecutar seleccionada con éxito");
		return unindadProximaAEjecutar;
	}
}

void *ObtenerProximoMasCorto(t_list *lista) {

	// aqui utilizaria list_sort para ordenar la lista por la rafaga est actual
	// mas corta y luego proximoAEjecutar simplemente tomaria el mas corto de esta.
	list_sort(lista, unidadConRafagaEstimadaMasCorta);
	return list_get(lista, 0);

}

bool unidadConRafagaEstimadaMasCorta(void* elem1, void* elem2) {

	UnidadPlanificable* unidad1 = (UnidadPlanificable*) elem1;
	UnidadPlanificable* unidad2 = (UnidadPlanificable*) elem2;


	return unidad1->infoUltimaEjecucion.est_raf_actual <= unidad2->infoUltimaEjecucion.est_raf_actual;

	// esta función retorna true si la rafaga estimada de elem1 es menos o igual a la de elem2
}

double calcularEstimacion(UnidadPlanificable *elem) {
	double estimacion;
	if (elem->infoUltimaEjecucion.primeraEstimacion == false) {
		estimacion = estInicial;
		elem->infoUltimaEjecucion.primeraEstimacion = true;
	} else {
		double est_ant = elem->infoUltimaEjecucion.est_raf_ant;
		double real_ant = elem->infoUltimaEjecucion.real_raf_ant;
		estimacion = ((1 - alpha) * est_ant) + (alpha * real_ant);
	}
	return estimacion;
}

void* asignarEstimacionAUnElemento(void* elem) {
	UnidadPlanificable* unidad = (UnidadPlanificable*) elem;
	unidad->infoUltimaEjecucion.est_raf_ant = unidad->infoUltimaEjecucion.est_raf_actual;
	// asignamos la actual anterior como est anterior
	unidad->infoUltimaEjecucion.est_raf_actual = calcularEstimacion(unidad);
	// calculamos el nuevo estimado y se lo asignamos
	return (void*) unidad;
}

void asignarEstimacionATodosLosElementosDeLaLista(t_list *list) {
	list_map(list, asignarEstimacionAUnElemento);
}

void obtenerConfiguracionesSJF() {
	t_config* configSJF = config_create(TEAM_CONFIG_FILE);

	alpha = config_get_double_value(configSJF, "ALPHA");
	estInicial = config_get_double_value(configSJF, "ESTIMACION_INICIAL");

	config_destroy(configSJF);
}

static void destruir(AlgoritmoPlanificador * this) {
	log_destroy(this->logger);
}

static AlgoritmoPlanificador new() {
	obtenerConfiguracionesSJF();
	return (AlgoritmoPlanificador ) {
		.logger = log_create(TEAM_INTERNAL_LOG_FILE, "AlgoritmoSJFsinDesalojo", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
		.tipo = SJF_SD,
		&proximoAEjecutar,
		&destruir
	} ;
}

const struct ShortestJobFirstClass ShortestJobFirstConstructor = {.new=&new};

