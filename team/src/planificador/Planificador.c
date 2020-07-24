//
// Created by Alan Zhao on 11/07/2020.
//

#include "planificador/Planificador.h"

void agregarUnidadesPlanificables(Planificador * this, t_list * unidadesPlanificables) {
    list_add_all(this->colas->colaNew, unidadesPlanificables);
}

void agregarUnidadPlanificable(Planificador * this, UnidadPlanificable * unidadPlanificable) {
    list_add(this->colas->colaNew, unidadPlanificable);
}

t_list* armarListaEntrenadoresDisponibles(Planificador * this) {
	// Retorna los de New, Blocked sin Espera y no llenos.
	t_list* entrenadoresDisponibles = list_duplicate(this->colas->colaNew); // Si hago duplicate a lista vacia, me retorna lista vacia?
	t_list* auxiliarBlocked = list_duplicate(this->colas->colaBlocked);
	for (int a = 0; a < list_size(auxiliarBlocked); a++) {
		HiloEntrenadorPlanificable* elementoIterado = (HiloEntrenadorPlanificable*)list_get(auxiliarBlocked,a);
		if (!elementoIterado->entrenador->estaEsperandoAlgo && elementoIterado->entrenador->puedeAtraparPokemones(elementoIterado->entrenador)){
			list_add(entrenadoresDisponibles, elementoIterado);
		}
	}
	list_destroy(auxiliarBlocked);
	return entrenadoresDisponibles;

}

UnidadPlanificable* obtenerProximoAEjecutar(Planificador * this) {
	return this->algoritmoPlanificador.proximoAEjecutar(&this->algoritmoPlanificador, this->colas->colaReady);
}

int cantidadDeRafagas(Planificador * planificador, UnidadPlanificable * unidadPlanificable){
	// segun algoritmo calcular numero
	int cantRafagas;

	switch(planificador->algoritmoPlanificador.tipo){
	case FIFO://FIFO
		cantRafagas = unidadPlanificable->tareaAsignada->totalInstrucciones;
		break;
	case RR://RR
		;
		int quantum = servicioDeConfiguracion.obtenerEntero(&servicioDeConfiguracion,QUANTUM);
		cantRafagas = minimo(quantum,(unidadPlanificable->tareaAsignada->
				cantidadInstruccionesRestantes(unidadPlanificable->tareaAsignada)));
		break;
	case SJF_CD://SJF-CD
		cantRafagas = 1;
		break;
	case SJF_SD://SJF-SD
		cantRafagas = unidadPlanificable->tareaAsignada->totalInstrucciones;
		break;
	default:
		cantRafagas = 0;
		log_error(INTERNAL_LOGGER,"algoritmo planificador invalido");
	}
	return cantRafagas;
}

int minimo(int nro1,int nro2){
	int min;
	if(nro1 <= nro2){
		min = nro1;
	}else{
		min = nro2;
	}
	return min;
}

void moverACola(Planificador * this,UnidadPlanificable * uPlanificable,EstadoPlanificador * estado,char* motivoCambio){
	t_list* colaDestino, colaOrigen;
	colaDestino = colaSegunEstado(this->colas,estado);
	colaOrigen = colaEnLaQueSeEncuentraLaUnidadPlanificable(this,uPlanificable);


	log_info(MANDATORY_LOGGER,"Se movio al entrenador con ID %s, a la cola %s. Motivo: %s",
			uPlanificable->entrenador->id,estado,motivoCambio);
}

bool entrenadorEstaEnLista(UnidadPlanificable * unidadParametro, void* elem){
	UnidadPlanificable unidadDeLista = (UnidadPlanificable*) elem;
	return unidadParametro->entrenador->id == unidadDeLista->entrenador->id;
}

t_list* colaEnLaQueSeEncuentraLaUnidadPlanificable(Planificador * planificador, UnidadPlanificable * uPlanif){
	t_list * cola;

	if(list_any_satisfy(planificador->colas->colaNew,entrenadorEstaEnLista(uPlanif))){
			cola = planificador->colas->colaNew;
		}
	if(list_any_satisfy(planificador->colas->colaReady,entrenadorEstaEnLista(uPlanif))){
		cola = planificador->colas->colaReady;
		}
	if(list_any_satisfy(planificador->colas->colaExec,entrenadorEstaEnLista(uPlanif))){
		cola = planificador->colas->colaExec;
		}
	if(list_any_satisfy(planificador->colas->colaBlocked,entrenadorEstaEnLista(uPlanif))){
		cola = planificador->colas->colaBlocked;
		}
	if(list_any_satisfy(planificador->colas->colaExit,entrenadorEstaEnLista(uPlanif))){
		cola = planificador->colas->colaExit;
		}
	return cola;
}

t_list* colaSegunEstado(ColasDePlanificacion colas,EstadoPlanificador estado){
	t_list* cola;
	switch(estado){
	case NEW_:
		cola = colas.colaNew;
		break;
	case 1:
		cola = colas.colaReady;
		break;
	case 2:
		cola = colas.colaExec;
		break;
	case 3:
		cola = colas.colaBlocked;
		break;
	case 4:
		cola = colas.colaExit;
		break;
	}
	return cola;
}


void destruirPlanificador(Planificador * this, void (*destructorUnidadPlanificable)(UnidadPlanificable *)) {
    log_destroy(this->logger);
    this->transicionadorDeEstados.destruir(&this->transicionadorDeEstados);
    destruirColasDePlanificacion(this->colas, destructorUnidadPlanificable);
}

static Planificador new(ServicioDeMetricas* servicio) { // TODO: asignar servicioDeMetricas que tiene que llegar por parametro.
    t_log * logger = log_create(TEAM_INTERNAL_LOG_FILE, "Planificador", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);

    char * nombreAlgoritmo = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, ALGORITMO_PLANIFICACION);
    log_info(logger, "El planificador se inicializará con el algoritmo %s", nombreAlgoritmoCompleto(nombreAlgoritmo));

    Planificador planificador = {
            .logger = logger,
            .algoritmoPlanificador = obtenerAlgoritmo(nombreAlgoritmo),
            .transicionadorDeEstados = TransicionadorDeEstadosConstructor.new(),
            .colas = crearColasDePlanificacion(),
			.servicioDeMetricas = servicio,
            &agregarUnidadesPlanificables,
            &agregarUnidadPlanificable,

			&armarListaEntrenadoresDisponibles,
			&obtenerProximoAEjecutar,
			&cantidadDeRafagas,

			&moverACola,
            &destruirPlanificador,
    };

    return planificador;
}

const struct PlanificadorClass PlanificadorConstructor = {.new=&new};
