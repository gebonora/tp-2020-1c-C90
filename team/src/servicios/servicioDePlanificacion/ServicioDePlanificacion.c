//
// Created by Alan Zhao on 11/07/2020.
//

#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"

void trabajar(ServicioDePlanificacion * this) {
	while (!this->finDeTrabajo) {

		sem_wait(&this->semaforoEjecucionHabilitada);

		if (this->finDeTrabajo) {
			log_debug(this->logger, "Se interrumpió el ciclo de trabajo por fin de trabajo");
			break;
		}

		// Cambiar desde acá.
		// TODO: Agarrar a los blocked y fijarnos si finalizaron.
		// Puede pasar que justo les queda un catch para terminar y les llega -> exit
		// O hacen pasivamente un intercambio y terminan -> exit
		for (int a = 0; a < list_size(this->planificador.colas->colaBlocked); a++) {
			HiloEntrenadorPlanificable* hilo = (HiloEntrenadorPlanificable*) list_get(this->planificador.colas->colaBlocked, a);
			if (hilo->entrenador->objetivoCompletado(hilo->entrenador)) {
				this->planificador.moverACola(&this->planificador, hilo, EXIT, "Estaba en espera de un evento para finalizar, y llegó.");
			}
		}

		if (this->teamFinalizado(this)) {
			this->finDeTrabajo = true;
		}

		if (this->evaluarEstadoPosibleDeadlock(this)) {
			// SI HAY DEADLOCK PLANIFICAMOS INTERCAMBIOS
			log_debug(this->logger, "Se detectó un estado de posible deadlock.");
			t_list* listaDeBloqueados = this->planificador.colas->colaBlocked;
			t_list* listaDeIntercambios = this->servicioDeResolucionDeDeadlocks->procesarDeadlock(this->servicioDeResolucionDeDeadlocks, listaDeBloqueados);
			this->asignarIntercambios(this, listaDeIntercambios);
		}

		else {
			// SI NO HAY DEADLOCK PLANIFICAMOS CAPTURAS
			log_debug(this->logger, "Se planificarán capturas.");
			// Me retorna los de New, y los de blocked que no estan en captura y no estan llenos
			// Esto de llenos lo metí porque no queremos planificarles capturas, eventualmente todos van a estar llenos y vamos a entrar al IF de deadlock.

			t_list* entrenadoresDisponibles = this->planificador.armarListaEntrenadoresDisponibles(&this->planificador);

			sem_wait(&this->semaforoContadorColaDeTrabajo);
			pthread_mutex_lock(&this->mutexColaDeTrabajo);
			t_list* trabajo = this->obtenerTrabajo(this, list_size(entrenadoresDisponibles));
			pthread_mutex_unlock(&this->mutexColaDeTrabajo);

			for (int a = 0; list_size(trabajo) - 1; a++) {
				sem_wait(&this->semaforoContadorColaDeTrabajo);
			}
			this->asignarTareasDeCaptura(this, trabajo, entrenadoresDisponibles);

		}
		HiloEntrenadorPlanificable* aEjecutar = this->planificador.obtenerProximoAEjecutar(&this->planificador);
		int ciclosAEjecutar = this->planificador.cantidadDeRafagas(&this->planificador, aEjecutar);

		if (this->ultimoHiloEjecutado->entrenador->id == NULL || string_equals(this->ultimoHiloEjecutado->entrenador->id, aEjecutar->entrenador->id)) {
			this->servicioDeMetricas->registrarCambioDeContexto(this->servicioDeMetricas);
			this->ultimoHiloEjecutado = aEjecutar;
		}

		this->planificador.moverACola(&this->planificador, aEjecutar, EXEC, "Ejecutará en el procesador.");
		aEjecutar->ejecutarParcialmente(aEjecutar, ciclosAEjecutar);

		for (int a = 0; a < ciclosAEjecutar; a++) {
			this->servicioDeMetricas->registrarCicloRealizadoPorEntrenador(this->servicioDeMetricas, aEjecutar->entrenador->id);
		}

		this->definirYCambiarEstado(this, aEjecutar); // Lo pasa a Ready si no terminó su tarea, Blocked o Exit si terminó su tarea.

		// Hasta acá.
	}
	sem_post(&this->semaforoFinDeTrabajo);
}

void asignarEquipoAPlanificar(ServicioDePlanificacion * this, Equipo equipo) {
	log_debug(this->logger, "Convirtiendo a los entrenadores en unidades planificables...");
	t_list * unidadesPlanificables = convertirAUnidadesPlanificables(equipo);
	this->planificador.agregarUnidadesPlanificables(&this->planificador, unidadesPlanificables);
	log_info(this->logger, "Los entrenadores fueron enviados a la cola de NEW");
	list_destroy(unidadesPlanificables);
}

t_list* obtenerTrabajo(ServicioDePlanificacion* this, int cantidadAPopear) {
// La idea es que tenemos una listaConEntrenadores y queremos popear hasta esa cantidad.
// Puede ser que popemos menos porque la cola tiene menor cantidad de elementos que los que pedimos, pero no pasa nada.
	t_list* listaDeTareas = list_create();
	int count = 0;
	while (!queue_is_empty(this->colaDeTrabajo) && count < cantidadAPopear) {
		list_add(listaDeTareas, queue_pop(this->colaDeTrabajo));
		count++;
	}
	return listaDeTareas;
}

void asignarTareasDeCaptura(ServicioDePlanificacion* this, t_list* tareas, t_list* entrenadoresDisponibles) {
	for (int a = 0; a < list_size(tareas); a++) {
		TrabajoPlanificador* trabajo = (TrabajoPlanificador*) list_get(tareas, a);
		HiloEntrenadorPlanificable* hiloElegido = devolverEntrenadorMasCercano(trabajo->coordenadaObjetivo, entrenadoresDisponibles);
		TareaPlanificable* tarea = generarTareaDeCaptura(hiloElegido->entrenador, trabajo->objetivo, trabajo->coordenadaObjetivo);
		hiloElegido->asignarTarea(hiloElegido, tarea);
		hiloElegido->entrenador->estaEsperandoAlgo = true;
		hiloElegido->infoUltimaEjecucion.seNecesitaNuevaEstimacion = true;
		hiloElegido->infoUltimaEjecucion.rafaga_real_actual = hiloElegido->tareaAsignada->totalInstrucciones;

		this->planificador.moverACola(&this->planificador, hiloElegido, READY, "Se le asignó una tarea de captura.");
		free(trabajo->objetivo);
		free(trabajo);
	}
	return;
}

void asignarIntercambios(ServicioDePlanificacion* this, t_list* intercambios) {
	for (int a = 0; a < list_size(intercambios); a++) {
		Intercambio* intercambio = (Intercambio*) list_get(intercambios, a);

		TareaPlanificable* tarea = generarTareaDeIntercambio(intercambio->entrenadorQueSeMueve->entrenador, intercambio->entrenadorQueEspera->entrenador,
				intercambio->pokemonQueObtieneElQueSeMueve, intercambio->pokemonQueObtieneElQueEspera);
		intercambio->entrenadorQueSeMueve->asignarTarea(intercambio->entrenadorQueSeMueve, tarea);
		intercambio->entrenadorQueSeMueve->entrenador->estaEsperandoAlgo = true;
		intercambio->entrenadorQueEspera->entrenador->estaEsperandoAlgo = true;

		intercambio->entrenadorQueSeMueve->infoUltimaEjecucion.seNecesitaNuevaEstimacion = true;
		intercambio->entrenadorQueSeMueve->infoUltimaEjecucion.rafaga_real_actual = intercambio->entrenadorQueSeMueve->tareaAsignada->totalInstrucciones;

		this->planificador.moverACola(&this->planificador, intercambio->entrenadorQueSeMueve, READY, "Se le asignó una tarea de intercambio.");

		free(intercambio->pokemonQueObtieneElQueEspera);
		free(intercambio->pokemonQueObtieneElQueSeMueve);
		free(intercambio);
	}
}

void definirYCambiarEstado(ServicioDePlanificacion* this, UnidadPlanificable* hilo) {
	if (hilo->tareaAsignada->cantidadInstruccionesEjecutadas(hilo->tareaAsignada) < hilo->tareaAsignada->totalInstrucciones) {
		this->planificador.moverACola(&this->planificador, hilo, READY, "Terminó una ráfaga y aún le quedan ciclos pendientes.");
		return;
	}
	if (hilo->tareaAsignada->cantidadInstruccionesEjecutadas(hilo->tareaAsignada) > hilo->tareaAsignada->totalInstrucciones) {
		log_error(this->logger, "El entrenador '%s' ejecutó más de lo que debía", hilo->entrenador->id);
		return;
	}
	if (hilo->entrenador->objetivoCompletado(hilo->entrenador)) {
		this->planificador.moverACola(&this->planificador, hilo, EXIT, "Completó su objetivo personal.");
		return;
	}
	if (hilo->entrenador->puedeAtraparPokemones(hilo->entrenador)) {
		this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera del resultado de una captura.");
		return;
	}
	this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un intercambio para poder terminar su objetivo personal.");
}

bool teamFinalizado(ServicioDePlanificacion* this) {
	return !list_is_empty(this->planificador.colas->colaExit) && list_is_empty(this->planificador.colas->colaReady) && list_is_empty(this->planificador.colas->colaExec)
			&& list_is_empty(this->planificador.colas->colaBlocked) && list_is_empty(this->planificador.colas->colaNew);
}

bool evaluarEstadoPosibleDeadlock(ServicioDePlanificacion* this) {
// el if loco. Buscamos un entrenador en blocked que no pueda capturar

// Cola NEW, y EXEC VACIO
	if (!list_is_empty(this->planificador.colas->colaNew) || !list_is_empty(this->planificador.colas->colaExec)) {
		return false;
	}

	bool entrenadorEnDeadlock(void* elem) {
		HiloEntrenadorPlanificable* hilo = (HiloEntrenadorPlanificable*) elem;
		return !hilo->entrenador->puedeAtraparPokemones(hilo->entrenador) && !hilo->entrenador->objetivoCompletado(hilo->entrenador);
	}

// Cola Blocked con todos en deadlock
	if (!list_all_satisfy(this->planificador.colas->colaBlocked, entrenadorEnDeadlock)) {
		return false;
	}
// Cola Ready con todos en deadlock
	if (!list_all_satisfy(this->planificador.colas->colaReady, entrenadorEnDeadlock)) {
		return false;
	}
	return true;
}

void destruir(ServicioDePlanificacion * this) {
	this->finDeTrabajo = true;
	sem_post(&this->semaforoEjecucionHabilitada);
	sem_wait(&this->semaforoFinDeTrabajo);
	log_debug(this->logger, "Se procede a destruir al servicio de planificacion");
	queue_destroy(this->colaDeTrabajo);
	log_destroy(this->logger);
	this->planificador.destruir(&this->planificador, destruirUnidadPlanificable);
	free(this);
}

static ServicioDePlanificacion * new(ServicioDeMetricas* servicioDeMetricas, ServicioDeResolucionDeDeadlocks* servicioDeadlocks) { // TODO arreglar esto para que planificador tenga servicio de metricas
	ServicioDePlanificacion * servicio = malloc(sizeof(ServicioDePlanificacion));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDePlanificacion", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
	servicio->colaDeTrabajo = queue_create();
	servicio->finDeTrabajo = false;
	sem_init(&servicio->semaforoFinDeTrabajo, 1, 0);
	sem_init(&servicio->semaforoEjecucionHabilitada, 1, 0);
	servicio->planificador = PlanificadorConstructor.new(servicioDeMetricas);
	pthread_mutex_init(&servicio->mutexColaDeTrabajo, NULL);
	sem_init(&servicio->semaforoContadorColaDeTrabajo, 0, 0); // Arranca en 0, queremos que el productor meta algo para consumir.
	servicio->ultimoHiloEjecutado = NULL;
	servicio->asignarEquipoAPlanificar = &asignarEquipoAPlanificar;
	servicio->asignarIntercambios = &asignarIntercambios;
	servicio->servicioDeResolucionDeDeadlocks = servicioDeadlocks;
	servicio->servicioDeMetricas = servicioDeMetricas;
	servicio->trabajar = &trabajar;
	servicio->obtenerTrabajo = &obtenerTrabajo;
	servicio->asignarTareasDeCaptura = &asignarTareasDeCaptura;
	servicio->definirYCambiarEstado = &definirYCambiarEstado;
	servicio->teamFinalizado = &teamFinalizado;
	servicio->evaluarEstadoPosibleDeadlock = &evaluarEstadoPosibleDeadlock;
	servicio->destruir = &destruir;

	crearHilo((void *(*)(void *)) servicio->trabajar, servicio);

	return servicio;
}

const struct ServicioDePlanificacionClass ServicioDePlanificacionConstructor = { .new = &new };

t_list * convertirAUnidadesPlanificables(Equipo equipo) {
	return list_map(equipo, (void *(*)(void *)) HiloEntrenadorPlanificableConstructor.new);
}

void destruirUnidadPlanificable(UnidadPlanificable * unidadPlanificable) {
	unidadPlanificable->destruir(unidadPlanificable);
}

HiloEntrenadorPlanificable* devolverEntrenadorMasCercano(Coordinate coor, t_list* hilos) {
	int max = -1;
	int posicionEnListaDelMaximo = 0;
	HiloEntrenadorPlanificable* puntero;
	for (int a = 0; a < list_size(hilos); a++) {
		HiloEntrenadorPlanificable* hilo = (HiloEntrenadorPlanificable*) list_get(hilos, a);
		int distancia = distanciaEntre(coor, hilo->entrenador->gps->posicionActual(hilo->entrenador->gps).coordenada);
		if (distancia > max) {
			puntero = hilo;
			max = distancia;
			posicionEnListaDelMaximo = a;
		}
	}
	list_remove(hilos, posicionEnListaDelMaximo);
	return puntero;
}

