//
// Created by Alan Zhao on 11/07/2020.
//
#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"

typedef struct EntrenadorConPokemon {
	PokemonAtrapable* pokemon;
	int distancia;
	HiloEntrenadorPlanificable* entrenadore;
} EntrenadorConPokemon;

static EntrenadorConPokemon* entrenador_optimo(ServicioDePlanificacion* this, t_list* pokemones, t_list* entrenadores);

void planificadorDeCapturas(ServicioDePlanificacion * this) {
	log_debug(this->logger, "Hilo de Planificador de capturas creado.");
	sem_wait(&this->semaforoEjecucionHabilitadaCapturas); // VOLARLOOO
	while (!this->finDeTrabajoCapturas) {
		log_debug(this->logger, "Iniciando planificación de capturas.");
		// Deberia arrancar cuando tengo un pokemon en el mapa y cuando tengo un entrenador en planificable.
		// wait(semaforoContadorPokemones);
		// wait(semaforoContadorEntrenadoresPlanificables);
		// Cuando pasamos un entrenadors a exit hacemo signal al objetivoGlobalCompletado, que fue inicializado con la cantidad de entrenadores.
		// Otra opción para esto: cuando pasamos a exit, llamamos a una funcion que revisa que la cola exit este llena y hago un post.
		//sem_wait(&semaforoCaptura);
		/*if (this->finDeTrabajoCapturas) {
		 log_debug(this->logger, "Se interrumpió el ciclo de trabajo por fin de trabajo");
		 break;
		 }*/
		t_list* entrenadoresDisponibles = this->planificador.armarListaEntrenadoresDisponibles(&this->planificador);
		t_list* trabajo = this->servicioDeCaptura->pokemonesDisponibles(this->servicioDeCaptura);
		this->asignarTareasDeCaptura(this, trabajo, entrenadoresDisponibles);
	}
	sem_post(&this->semaforoFinDeTrabajoCapturas); // TODO: Sacar estos.
}

void planificadorCortoPlazo(ServicioDePlanificacion* this) {
	log_debug(this->logger, "Hilo de Planificador de corto plazo creado.");
	sem_wait(&this->semaforoEjecucionHabilitadaCortoPlazo); // VOLARLO ALLAHU AKABAR
	while (!this->finDeTrabajoCortoPlazo) {

		// Cuando tengo un chabon en la cola  de ready.

		sem_wait(&semaforoReady);

		// un post cuando paso un hilo a ready. ponerlo en el planificador->moverACola.

		if (this->finDeTrabajoCortoPlazo) { // VOLARLO AL JORACA
			log_debug(this->logger, "Se interrumpió el ciclo de trabajo por fin de trabajo");
			break;
		}

		HiloEntrenadorPlanificable* aEjecutar = this->planificador.obtenerProximoAEjecutar(&this->planificador);
		int ciclosAEjecutar = this->planificador.cantidadDeRafagas(&this->planificador, aEjecutar);

		log_info(this->logger, "va a ejecutar %s por %d.", aEjecutar->entrenador->id, ciclosAEjecutar);

		this->servicioDeMetricas->registrarCambioDeContexto(this->servicioDeMetricas); // TODO: pensarlo bien
		this->planificador.moverACola(&this->planificador, aEjecutar, EXEC, "Ejecutará en el procesador.");
		this->servicioDeMetricas->registrarCambioDeContexto(this->servicioDeMetricas);
		aEjecutar->ejecutarParcialmente(aEjecutar, ciclosAEjecutar);

		for (int a = 0; a < ciclosAEjecutar; a++) {
			this->servicioDeMetricas->registrarCicloRealizadoPorEntrenador(this->servicioDeMetricas, aEjecutar->entrenador->id);
		}

		this->definirYCambiarEstado(this, aEjecutar); // Lo pasa a Ready si no terminó su tarea, Blocked o Exit si terminó su tarea.
	}
	sem_post(&this->semaforoFinDeTrabajoCortoPlazo);
}

void planificadorDeadlocks(ServicioDePlanificacion* this) {
	log_debug(this->logger, "Hilo de Planificador de deadlocks creado.");
	while (!this->finDeTrabajoDeadlock) {
		sem_wait(&semaforoDeadlock);

		if (this->teamFinalizado(this)) { // TODO: vamos a usar un semaforo que postea cuando se cambio a exit.
			log_debug(this->logger, "Finalizando planificacion");
			this->finDeTrabajoDeadlock = true;
			sem_post(&semaforoObjetivoGlobalCompletado);
			continue;
		}

		// Debería postear cuando pasa una tarea a blocked, a exit.
		// Cuando llega un caught;

		if (this->finDeTrabajoDeadlock) { // EXPLOTARLO A LA M
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
	}
	sem_post(&this->semaforoFinDeTrabajoDeadlock);
}

void asignarEquipoAPlanificar(ServicioDePlanificacion * this, Equipo equipo) {
	log_debug(this->logger, "Convirtiendo a los entrenadores en unidades planificables...");
	t_list * unidadesPlanificables = convertirAUnidadesPlanificables(equipo);

	this->planificador.agregarUnidadesPlanificables(&this->planificador, unidadesPlanificables);
	log_info(this->logger, "Los entrenadores fueron enviados a la cola de NEW");
	list_destroy(unidadesPlanificables);
}

static EntrenadorConPokemon* entrenador_optimo(ServicioDePlanificacion* this, t_list* pokemones, t_list* entrenadores) {
	bool pokemon_capturable(void* elem) {
		PokemonAtrapable* poke = (PokemonAtrapable*) elem;
		return this->objetivoGlobal.puedeCapturarse(&this->objetivoGlobal, poke->especie);
	}

	t_list* pokemones_capturables = list_filter(pokemones, pokemon_capturable);

	if (list_size(pokemones_capturables) > 0) {
		t_list* lista = list_create();
		log_info(this->logger, "Tengo %d, pokemones capturables", list_size(pokemones_capturables));

		log_info(this->logger, "Tengo %d entrenadores", list_size(entrenadores));

		for (int index = 0; index < list_size(entrenadores); index++) {

			HiloEntrenadorPlanificable* hiloElegido = (HiloEntrenadorPlanificable*) list_get(entrenadores, index);
			Coordinate posicion_entrenador = hiloElegido->entrenador->gps->posicionActual(hiloElegido->entrenador->gps).coordenada;

			bool masCercano(void* elem1, void* elem2) {
				PokemonAtrapable* pokemon1 = (PokemonAtrapable*) elem1;
				PokemonAtrapable* pokemon2 = (PokemonAtrapable*) elem2;

				Coordinate coor1 = pokemon1->gps->posicionActual(pokemon1->gps).coordenada;
				Coordinate coor2 = pokemon2->gps->posicionActual(pokemon2->gps).coordenada;
				log_info(this->logger, "La distancia del entrenador %s al pokemon %s es de %d, y al %s es de %d", hiloElegido->entrenador->id, pokemon1->especie,
						distanciaEntre(posicion_entrenador, coor1), pokemon2->especie, distanciaEntre(posicion_entrenador, coor2));
				return distanciaEntre(posicion_entrenador, coor1) <= distanciaEntre(posicion_entrenador, coor2);
			}

			list_sort(pokemones, masCercano);

			PokemonAtrapable* pokemon = list_get(pokemones, 0);

			EntrenadorConPokemon* competidor = malloc(sizeof(EntrenadorConPokemon));
			competidor->pokemon = pokemon;
			competidor->entrenadore = hiloElegido;
			competidor->distancia = distanciaEntre(posicion_entrenador, pokemon->gps->posicionActual(pokemon->gps).coordenada);

			list_add(lista, competidor);
		}

		bool mas_piola(void* elem1, void* elem2) {
			EntrenadorConPokemon* e1 = (EntrenadorConPokemon*) elem1;
			EntrenadorConPokemon* e2 = (EntrenadorConPokemon*) elem2;

			return e1->distancia < e2->distancia;
		}

		list_sort(lista, mas_piola);

		void print_entr(EntrenadorConPokemon* entrendaro) {
			log_info(this->logger, "Paso a la final: Entrenador %s, Pokemon %s, Distancia %d", entrendaro->entrenadore->entrenador->id, entrendaro->pokemon->especie,
					entrendaro->distancia);
		}

		list_iterate(lista, (void (*)(void*)) print_entr);

		EntrenadorConPokemon* ganador = list_get(lista, 0);

		log_info(this->logger, "Trajo la copa: Entrenador %s, Pokemon %s, Distancia %d", ganador->entrenadore->entrenador->id, ganador->pokemon->especie,
				ganador->distancia);

		list_destroy(lista);
		list_destroy(pokemones_capturables);

		return ganador;
	}

	return NULL;
}

void asignarTareasDeCaptura(ServicioDePlanificacion* this, t_list* listaPokemon, t_list* entrenadoresDisponibles) {
	pthread_mutex_lock(&messi); // volarlo
	log_info(this->logger, "Asignando tareas de captura");
	int iteracionesPosibles = MIN(list_size(listaPokemon), list_size(entrenadoresDisponibles));
	for (int i = 0; i < iteracionesPosibles; i++) {
		EntrenadorConPokemon* ganador = entrenador_optimo(this, listaPokemon, entrenadoresDisponibles);

		if (ganador != NULL) {
			HiloEntrenadorPlanificable* hiloElegido = (HiloEntrenadorPlanificable*) ganador->entrenadore;
			log_info(this->logger, "Ya tengo un entrenador optimo");
			PokemonAtrapable* pokemon = ganador->pokemon;
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

			bool pokemon_igual_a(void* elem) {
				PokemonAtrapable * poke = (PokemonAtrapable*) elem;
				return string_equals_ignore_case(poke->especie, pokemon->especie);
			}

			list_remove_by_condition(listaPokemon, pokemon_igual_a);
			//log_info(this->logger, "Gol de Messi");

			bool entrenador_by_id(void* elem) {
				HiloEntrenadorPlanificable* entrenador_actual = (HiloEntrenadorPlanificable*) elem;
				return string_equals_ignore_case(entrenador_actual->entrenador->id, hiloElegido->entrenador->id);
			}

			list_remove_by_condition(entrenadoresDisponibles, entrenador_by_id);
		}
		free(ganador);
	}
	list_destroy(listaPokemon);

	for (int g = 0; g > iteracionesPosibles - 1; g++) {
		//wait(&semaforoContadorPokemons);
		//wait(&semaforoContadorEntrenadoresDisponibles);
	}
	pthread_mutex_unlock(&messi); // Volar

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
		intercambio->entrenadorQueSeMueve->infoUltimaEjecucion.totalTarea = intercambio->entrenadorQueSeMueve->tareaAsignada->totalInstrucciones;

		pthread_mutex_lock(&mtxReady); // TODO: sacar a otro lado
		this->planificador.moverACola(&this->planificador, intercambio->entrenadorQueSeMueve, READY, "Se le asignó una tarea de intercambio.");
		pthread_mutex_unlock(&mtxReady);

		this->servicioDeMetricas->registrarIntercambio(this->servicioDeMetricas, intercambio->entrenadorQueSeMueve->entrenador->id);
		sem_post(&semaforoReady); // mover a planificaidor -> cmoverACola
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
			sem_post(&semaforoPokemone);
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
	if (hilo->tareaAsignada != NULL) {
		if (hilo->tareaAsignada->cantidadInstruccionesEjecutadas(hilo->tareaAsignada) < hilo->tareaAsignada->totalInstrucciones) {
			pthread_mutex_lock(&mtxReady);
			this->planificador.moverACola(&this->planificador, hilo, READY, "Terminó una ráfaga y aún le quedan ciclos pendientes.");
			pthread_mutex_unlock(&mtxReady);
			sem_post(&semaforoReady);
			return;
		}
	}
	if (hilo->entrenador->puedeAtraparPokemones(hilo->entrenador)) {
		pthread_mutex_lock(&mtxBlock);
		this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un evento.");
		pthread_mutex_unlock(&mtxBlock);
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

bool teamFinalizado(ServicioDePlanificacion* this) { // TODO: cuando pasamos un entrenador a exit. llamamos a esta y que haga el signal
	return !list_is_empty(this->planificador.colas->colaExit) && list_is_empty(this->planificador.colas->colaReady) && list_is_empty(this->planificador.colas->colaExec)
			&& list_is_empty(this->planificador.colas->colaBlocked) && list_is_empty(this->planificador.colas->colaNew);
}

bool evaluarEstadoPosibleDeadlock(ServicioDePlanificacion* this) { // TODO: pensar mass
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
	if (list_all_satisfy(this->planificador.colas->colaBlocked, entrenadorEnDeadlock) && list_is_empty(this->planificador.colas->colaReady)
			&& list_is_empty(this->planificador.colas->colaNew) && list_is_empty(this->planificador.colas->colaExec)) {
		return true;
	}
	return false;
}

void destruir(ServicioDePlanificacion * this) { // TODO: esto es parte del problema de como salir sin un exit(0)
	this->finDeTrabajoCapturas = true;
	sem_post(&this->semaforoEjecucionHabilitadaCapturas);
	sem_wait(&this->semaforoFinDeTrabajoCapturas);

	/*this->finDeTrabajo2 = true;
	 sem_post(&this->semaforoEjecucionHabilitada2);
	 sem_wait(&this->semaforoFinDeTrabajo2);
	 */

	log_debug(this->logger, "Se procede a destruir al servicio de planificacion");
	log_destroy(this->logger);
	this->planificador.destruir(&this->planificador, destruirUnidadPlanificable);
	free(this);
}

static ServicioDePlanificacion * new(ServicioDeMetricas* servicioDeMetricas, ServicioDeResolucionDeDeadlocks* servicioDeadlocks) { // TODO arreglar esto para que planificador tenga servicio de metricas
	ServicioDePlanificacion * servicio = malloc(sizeof(ServicioDePlanificacion));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDePlanificacion", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
	servicio->finDeTrabajoCapturas = false;
	servicio->finDeTrabajoCortoPlazo = false;
	servicio->finDeTrabajoDeadlock = false;
	sem_init(&servicio->semaforoFinDeTrabajoCapturas, 0, 0);
	sem_init(&servicio->semaforoFinDeTrabajoCortoPlazo, 0, 0);
	sem_init(&servicio->semaforoFinDeTrabajoDeadlock, 0, 0);
	sem_init(&servicio->semaforoEjecucionHabilitadaCapturas, 0, 0);
	sem_init(&servicio->semaforoEjecucionHabilitadaCortoPlazo, 0, 0);
	sem_init(&servicio->semaforoEjecucionHabilitadaDeadlock, 0, 0);
	servicio->planificador = PlanificadorConstructor.new(servicioDeMetricas);
	servicio->ultimoHiloEjecutado = NULL;
	servicio->asignarEquipoAPlanificar = &asignarEquipoAPlanificar;
	servicio->asignarIntercambios = &asignarIntercambios;
	servicio->servicioDeResolucionDeDeadlocks = servicioDeadlocks;
	servicio->servicioDeMetricas = servicioDeMetricas;
	servicio->planificadorDeCapturas = &planificadorDeCapturas;
	servicio->planificadorCortoPlazo = &planificadorCortoPlazo;
	servicio->planificadorDeadlocks = &planificadorDeadlocks;
	servicio->asignarTareasDeCaptura = &asignarTareasDeCaptura;
	servicio->definirYCambiarEstado = &definirYCambiarEstado;
	servicio->teamFinalizado = &teamFinalizado;
	servicio->evaluarEstadoPosibleDeadlock = &evaluarEstadoPosibleDeadlock;
	servicio->destruir = &destruir;

	crearHilo((void *(*)(void *)) servicio->planificadorDeCapturas, servicio);
	crearHilo((void *(*)(void *)) servicio->planificadorCortoPlazo, servicio);
	crearHilo((void *(*)(void *)) servicio->planificadorDeadlocks, servicio);

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

