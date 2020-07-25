//
// Created by Alan Zhao on 11/07/2020.
//
#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"

void trabajar(ServicioDePlanificacion * this) {
	log_debug(this->logger, "Hilo de planificación creado.");
	while (!this->finDeTrabajo) {
		sem_wait(&this->semaforoEjecucionHabilitada);

		if (this->finDeTrabajo) {
			log_debug(this->logger, "Se interrumpió el ciclo de trabajo por fin de trabajo");
			break;
		}
		log_debug(this->logger, "Iniciando planificación.");
		// Cambiar desde acá.
		// Puede pasar que justo les queda un catch para terminar y les llega -> exit
		// O hacen pasivamente un intercambio y terminan -> exit

		log_warning(this->logger, "Me clave en trabajar wait despues del else");
		sem_wait(&semaforoPokemone);
		sem_post(&semaforoPokemone);
		// SI NO HAY DEADLOCK PLANIFICAMOS CAPTURAS
		log_debug(this->logger, "Se planificarán capturas.");
		// Me retorna los de New, y los de blocked que no estan en captura y no estan llenos
		// Esto de llenos lo metí porque no queremos planificarles capturas, eventualmente todos van a estar llenos y vamos a entrar al IF de deadlock.
		t_list* entrenadoresDisponibles = this->planificador.armarListaEntrenadoresDisponibles(&this->planificador);

		t_list* trabajo = this->servicioDeCaptura->pokemonesDisponibles(this->servicioDeCaptura);
		this->asignarTareasDeCaptura(this, trabajo, entrenadoresDisponibles);

		sem_post(&this->semaforoEjecucionHabilitada);
	}
	sem_post(&this->semaforoFinDeTrabajo);
}

void trabajar2(ServicioDePlanificacion* this) {
	while (!this->finDeTrabajo2) {
		log_info(this->logger, "Iniciando planificacion de cola reaady");

		log_info(this->logger, "Clavado antes del foro trabajar2");
		sem_wait(&semaforoTrabajar2);

		if (this->finDeTrabajo2) {
			log_debug(this->logger, "Se interrumpió el ciclo de trabajo por fin de trabajo");
			break;
		}

		sem_wait(&semaforoReady);

		HiloEntrenadorPlanificable* aEjecutar = this->planificador.obtenerProximoAEjecutar(&this->planificador);
		int ciclosAEjecutar = this->planificador.cantidadDeRafagas(&this->planificador, aEjecutar);

		log_info(this->logger, "va a ejecutar %s por %d.", aEjecutar->entrenador->id, ciclosAEjecutar);

		if (this->ultimoHiloEjecutado == NULL || string_equals(this->ultimoHiloEjecutado->entrenador->id, aEjecutar->entrenador->id)) {
			this->servicioDeMetricas->registrarCambioDeContexto(this->servicioDeMetricas);
			this->ultimoHiloEjecutado = aEjecutar;
		}

		this->planificador.moverACola(&this->planificador, aEjecutar, EXEC, "Ejecutará en el procesador.");
		aEjecutar->ejecutarParcialmente(aEjecutar, ciclosAEjecutar);

		for (int a = 0; a < ciclosAEjecutar; a++) {
			this->servicioDeMetricas->registrarCicloRealizadoPorEntrenador(this->servicioDeMetricas, aEjecutar->entrenador->id);
		}

		this->definirYCambiarEstado(this, aEjecutar); // Lo pasa a Ready si no terminó su tarea, Blocked o Exit si terminó su tarea.
		sem_post(&semaforoTrabajar2);
	}
	sem_post(&this->semaforoFinDeTrabajo2);
}

void trabajar3(ServicioDePlanificacion* this) {
	puts("WENTRE A TABAJAR 3");
	while (!this->finDeTrabajo3) {
		sem_wait(&this->semaforoEjecucionHabilitada3);
		log_error(this->logger, "trabajar 3 paso semanforo");
		sem_wait(&semaforoDeadlock);

		puts("entre a aca");

		if (this->teamFinalizado(this)) {
			log_debug(this->logger, "Finalizando planificacion");
			this->finDeTrabajo = true;
			sem_post(&semaforoObjetivoGlobalCompletado);
			continue;
		}

		if (this->finDeTrabajo3) {
			log_debug(this->logger, "Se interrumpió el ciclo de trabajo por fin de trabajo");
			break;
		}

		for (int a = 0; a < list_size(this->planificador.colas->colaBlocked); a++) {
			HiloEntrenadorPlanificable* hilo = (HiloEntrenadorPlanificable*) list_get(this->planificador.colas->colaBlocked, a);
			if (hilo->entrenador->objetivoCompletado(hilo->entrenador)) {
				this->planificador.moverACola(&this->planificador, hilo, EXIT, "Estaba en espera de un evento para finalizar, y llegó.");
			}
		}

		if (this->evaluarEstadoPosibleDeadlock(this)) {
			// SI HAY DEADLOCK PLANIFICAMOS INTERCAMBIOS
			log_debug(this->logger, "Se detectó un estado de posible deadlock.");
			t_list* listaDeBloqueados = this->planificador.colas->colaBlocked;
			t_list* listaDeIntercambios = this->servicioDeResolucionDeDeadlocks->procesarDeadlock(this->servicioDeResolucionDeDeadlocks, listaDeBloqueados);
			this->asignarIntercambios(this, listaDeIntercambios);
		}

		sem_post(&this->semaforoEjecucionHabilitada3);
	}
	sem_post(&this->semaforoFinDeTrabajo3);
}

void asignarEquipoAPlanificar(ServicioDePlanificacion * this, Equipo equipo) {
	log_debug(this->logger, "Convirtiendo a los entrenadores en unidades planificables...");
	t_list * unidadesPlanificables = convertirAUnidadesPlanificables(equipo);

	printf("asginar equipo a  planif:%d \n\n", list_size(unidadesPlanificables));

	this->planificador.agregarUnidadesPlanificables(&this->planificador, unidadesPlanificables);
	log_info(this->logger, "Los entrenadores fueron enviados a la cola de NEW");
	list_destroy(unidadesPlanificables);
}

void asignarTareasDeCaptura(ServicioDePlanificacion* this, t_list* listaPokemon, t_list* entrenadoresDisponibles) {
// buscasr el mas cercano en el mapa -> si objetivo.puedeCapturarse -> si y le resto al objetivo
// else sigo iterando.
// Mapa me da pokemon de libs. lo sorteo por distancia desde mi entrenador.
// itero sobre esta lista con objetivo.puedeCapturarse. hasta encontrar un match.

// manejador de eventos
	log_info(this->logger, "Asignando tareas de captura");
	bool asigne = false;
	for (int a = 0; a < list_size(entrenadoresDisponibles); a++) {
		HiloEntrenadorPlanificable* hiloElegido = (HiloEntrenadorPlanificable*) list_get(entrenadoresDisponibles, a);
		Coordinate posicionEntrenador = hiloElegido->entrenador->gps->posicionActual(hiloElegido->entrenador->gps).coordenada;
		log_info(this->logger, "Antes de ordenar");
		for (int i = 0; i < list_size(listaPokemon); i++) {
			PokemonAtrapable * poke = list_get(listaPokemon, i);
			int distanciaA = distanciaEntre(posicionEntrenador, poke->posicion(poke).coordenada);
			log_info(this->logger, "Pokemon: %s, distancia: %d", poke->especie, distanciaA);
		}

		bool masCercano(void* elem1, void* elem2) {
			PokemonAtrapable* pokemon1 = (PokemonAtrapable*) elem1;
			Coordinate coor1 = pokemon1->posicion(pokemon1).coordenada;

			PokemonAtrapable* pokemon2 = (PokemonAtrapable*) elem2;
			Coordinate coor2 = pokemon2->posicion(pokemon2).coordenada;

			int a = distanciaEntre(posicionEntrenador, coor1);
			int b = distanciaEntre(posicionEntrenador, coor2);

			log_debug(this->logger, " pokemon 1 %s distancia %d // pokemon 2 %s distancia %d", pokemon1->especie, a, pokemon2->especie, b);
			return a < b;
		}
		list_sort(listaPokemon, &masCercano);
		log_info(this->logger, "Despues de ordenar");
		for (int i = 0; i < list_size(listaPokemon); i++) {
			PokemonAtrapable * poke = list_get(listaPokemon, i);
			int distanciaA = distanciaEntre(posicionEntrenador, poke->posicion(poke).coordenada);
			log_info(this->logger, "Pokemon: %s, distancia: %d", poke->especie, distanciaA);
		}

		for (int b = 0; b < list_size(listaPokemon); b++) {
			PokemonAtrapable* pokemon = (PokemonAtrapable*) list_get(listaPokemon, b);
			if (this->objetivoGlobal.puedeCapturarse(&this->objetivoGlobal, pokemon->especie)) {	// objetivo.puedeCapturarse.
				Coordinate coordenadaPokemon = pokemon->gps->posicionActual(pokemon->gps).coordenada;
				TareaPlanificable* tarea = generarTareaDeCaptura(hiloElegido->entrenador, pokemon->especie, coordenadaPokemon);
				hiloElegido->asignarTarea(hiloElegido, tarea);
				hiloElegido->entrenador->estaEsperandoAlgo = true;
				hiloElegido->infoUltimaEjecucion.seNecesitaNuevaEstimacion = true;
				hiloElegido->infoUltimaEjecucion.rafaga_real_actual = hiloElegido->tareaAsignada->totalInstrucciones;
				pokemon->marcarComoObjetivo(pokemon, hiloElegido->entrenador->id);
				this->objetivoGlobal.restarUnCapturado(&this->objetivoGlobal, pokemon->especie);
				pthread_mutex_lock(&mtxReady);
				this->planificador.moverACola(&this->planificador, hiloElegido, READY, "Se le asignó una tarea de captura.");
				pthread_mutex_unlock(&mtxReady);
				list_remove(listaPokemon, b);
				sem_wait(&semaforoPokemone);
				sem_post(&semaforoReady);
				asigne = true;
				break;
			}
		}
	}
	if (!asigne) {
		log_warning(this->logger, "Quedé clavado esperando");
		sem_wait(&semaforoPokemone);
	}
	list_destroy(listaPokemon);
}

void asignarIntercambios(ServicioDePlanificacion* this, t_list* intercambios) {
	for (int a = 0; a < list_size(intercambios); a++) {
		Intercambio* intercambio = (Intercambio*) list_get(intercambios, a);

		TareaPlanificable* tarea = generarTareaDeIntercambio(intercambio->entrenadorQueSeMueve->entrenador, intercambio->entrenadorQueEspera->entrenador,
				intercambio->pokemonQueObtieneElQueEspera, intercambio->pokemonQueObtieneElQueSeMueve);
		intercambio->entrenadorQueSeMueve->asignarTarea(intercambio->entrenadorQueSeMueve, tarea);
		intercambio->entrenadorQueSeMueve->entrenador->estaEsperandoAlgo = true;
		intercambio->entrenadorQueEspera->entrenador->estaEsperandoAlgo = true;

		intercambio->entrenadorQueSeMueve->infoUltimaEjecucion.seNecesitaNuevaEstimacion = true;
		intercambio->entrenadorQueSeMueve->infoUltimaEjecucion.rafaga_real_actual = intercambio->entrenadorQueSeMueve->tareaAsignada->totalInstrucciones;

		pthread_mutex_lock(&mtxReady);
		this->planificador.moverACola(&this->planificador, intercambio->entrenadorQueSeMueve, READY, "Se le asignó una tarea de intercambio.");
		pthread_mutex_unlock(&mtxReady);

		this->servicioDeMetricas->registrarIntercambio(this->servicioDeMetricas, intercambio->entrenadorQueSeMueve->entrenador->id);
		sem_post(&semaforoReady);
		free(intercambio->pokemonQueObtieneElQueEspera);
		free(intercambio->pokemonQueObtieneElQueSeMueve);
		free(intercambio);
	}
}

void definirYCambiarEstado(ServicioDePlanificacion* this, UnidadPlanificable* hilo) {
	if (hilo->tareaAsignada == NULL) {
		if (hilo->entrenador->estaEsperandoAlgo) {
			pthread_mutex_lock(&mtxBlock);
			this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un evento.");
			pthread_mutex_unlock(&mtxBlock);
			return;
		}
		if (!hilo->entrenador->objetivoCompletado(hilo->entrenador) && !hilo->entrenador->puedeAtraparPokemones(hilo->entrenador)) {
			pthread_mutex_lock(&mtxBlock);
			this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un intercambio.");
			pthread_mutex_unlock(&mtxBlock);
			return;
		}
	}
	if (hilo->entrenador->objetivoCompletado(hilo->entrenador)) {
		pthread_mutex_lock(&mtxExit);
		this->planificador.moverACola(&this->planificador, hilo, EXIT, "Completó su objetivo personal.");
		pthread_mutex_unlock(&mtxExit);
		return;
	}
	if (hilo->entrenador->puedeAtraparPokemones(hilo->entrenador)) {
		pthread_mutex_lock(&mtxBlock);
		this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un evento.");
		pthread_mutex_unlock(&mtxBlock);
		return;
	}
	if (hilo->tareaAsignada->cantidadInstruccionesEjecutadas(hilo->tareaAsignada) < hilo->tareaAsignada->totalInstrucciones) {
		pthread_mutex_lock(&mtxReady);
		this->planificador.moverACola(&this->planificador, hilo, READY, "Terminó una ráfaga y aún le quedan ciclos pendientes.");
		pthread_mutex_unlock(&mtxReady);
		sem_post(&semaforoReady);
		return;
	}
	if (hilo->tareaAsignada->cantidadInstruccionesEjecutadas(hilo->tareaAsignada) > hilo->tareaAsignada->totalInstrucciones) {
		log_error(this->logger, "El entrenador '%s' ejecutó más de lo que debía", hilo->entrenador->id);
		return;
	}
	pthread_mutex_lock(&mtxBlock);
	this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Terminó en un estado block.");
	pthread_mutex_unlock(&mtxBlock);
	sem_post(&semaforoReady);

}

bool teamFinalizado(ServicioDePlanificacion* this) {
	return !list_is_empty(this->planificador.colas->colaExit) && list_is_empty(this->planificador.colas->colaReady) && list_is_empty(this->planificador.colas->colaExec)
			&& list_is_empty(this->planificador.colas->colaBlocked) && list_is_empty(this->planificador.colas->colaNew);
}

bool evaluarEstadoPosibleDeadlock(ServicioDePlanificacion* this) {
// el if loco. Buscamos un entrenador en blocked que no pueda capturar

	printf("EVALUAR ESTADO COLA NEW: %d\n\n", list_size(this->planificador.colas->colaNew));
// Cola NEW, y EXEC VACIO
	if (!list_is_empty(this->planificador.colas->colaNew) || !list_is_empty(this->planificador.colas->colaExec)) {
		return false;
	}

	bool entrenadorEnDeadlock(void* elem) {
		HiloEntrenadorPlanificable* hilo = (HiloEntrenadorPlanificable*) elem;
		return !hilo->entrenador->puedeAtraparPokemones(hilo->entrenador) && !hilo->entrenador->objetivoCompletado(hilo->entrenador);
	}

// Cola Blocked con todos en deadlock
	if (list_all_satisfy(this->planificador.colas->colaBlocked, entrenadorEnDeadlock) && list_is_empty(this->planificador.colas->colaReady)
			&& list_is_empty(this->planificador.colas->colaNew) && list_is_empty(this->planificador.colas->colaExec)) {
		return true;
	}
	return false;
}

void destruir(ServicioDePlanificacion * this) {
	this->finDeTrabajo = true;
	sem_post(&this->semaforoEjecucionHabilitada);
	sem_wait(&this->semaforoFinDeTrabajo);
	log_debug(this->logger, "Se procede a destruir al servicio de planificacion");
	log_destroy(this->logger);
	this->planificador.destruir(&this->planificador, destruirUnidadPlanificable);
	free(this);
}

static ServicioDePlanificacion * new(ServicioDeMetricas* servicioDeMetricas, ServicioDeResolucionDeDeadlocks* servicioDeadlocks) { // TODO arreglar esto para que planificador tenga servicio de metricas
	ServicioDePlanificacion * servicio = malloc(sizeof(ServicioDePlanificacion));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDePlanificacion", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
	servicio->finDeTrabajo = false;
	servicio->finDeTrabajo2 = false;
	servicio->finDeTrabajo3 = false;
	sem_init(&servicio->semaforoFinDeTrabajo, 1, 0);
	sem_init(&servicio->semaforoFinDeTrabajo2, 1, 0);
	sem_init(&servicio->semaforoFinDeTrabajo3, 1, 0);
	sem_init(&servicio->semaforoEjecucionHabilitada, 1, 0); // TODO: aca q onda
	sem_init(&servicio->semaforoEjecucionHabilitada2, 1, 0); // TODO: aca q onda
	sem_init(&servicio->semaforoEjecucionHabilitada3, 1, 0); // TODO: aca q onda
	servicio->planificador = PlanificadorConstructor.new(servicioDeMetricas);
	servicio->ultimoHiloEjecutado = NULL;
	servicio->asignarEquipoAPlanificar = &asignarEquipoAPlanificar;
	servicio->asignarIntercambios = &asignarIntercambios;
	servicio->servicioDeResolucionDeDeadlocks = servicioDeadlocks;
	servicio->servicioDeMetricas = servicioDeMetricas;
	servicio->trabajar = &trabajar;
	servicio->trabajar2 = &trabajar2;
	servicio->trabajar3 = &trabajar3;
	servicio->asignarTareasDeCaptura = &asignarTareasDeCaptura;
	servicio->definirYCambiarEstado = &definirYCambiarEstado;
	servicio->teamFinalizado = &teamFinalizado;
	servicio->evaluarEstadoPosibleDeadlock = &evaluarEstadoPosibleDeadlock;
	servicio->destruir = &destruir;

	/*pthread_t hilito;
	 pthread_create(&hilito, NULL, (void *(*)(void*))servicio->trabajar2, servicio);
	 pthread_detach(hilito);
	 */
	crearHilo((void *(*)(void *)) servicio->trabajar, servicio);
	crearHilo((void *(*)(void *)) servicio->trabajar2, servicio);
	crearHilo((void *(*)(void *)) servicio->trabajar3, servicio);

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

