//
// Created by Alan Zhao on 11/07/2020.
//
#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"

void planificadorDeCapturas(ServicioDePlanificacion * this) {
	log_debug(this->logger, "Hilo de Planificador de capturas creado.");
	while (!this->finDeTrabajoCapturas) {
		sem_wait(&this->semaforoEjecucionHabilitadaCapturas);

		// Deberia arrancar cuando tengo un pokemon en el mapa y cuando tengo un entrenador en planificable.

		// Cuando pasamos un entrenadors a exit hacemo signal al objetivoGlobalCompletado, que fue inicializado con la cantidad de entrenadores.
		// Otra opción para esto: cuando pasamos a exit, llamamos a una funcion que revisa que la cola exit este llena y hago un post.

		sem_wait(&semaforoContadorPokemon);
		sem_wait(&semaforoContadorEntrenadoresDisponibles);

		if (this->finDeTrabajoCapturas) { // Esto permite al hilo morirse
			log_debug(this->logger, "Se terminó el hilo de Planificador de capturas");
			break;
		}
		log_debug(this->logger, "Iniciando planificación de capturas.");

		if (list_is_empty(this->servicioDeCaptura->pokemonesAtrapables) || list_is_empty(this->planificador.colas->colaBlocked)
				|| list_is_empty(this->planificador.colas->colaNew)) {
			log_error(this->logger, "Se quizó asignar capturas con listas vacías!!! (Si estás corriendo los tests es esperable.)");
			sem_post(&this->semaforoEjecucionHabilitadaCapturas);
			continue;
		}

		t_list* entrenadoresDisponibles = this->planificador.armarListaEntrenadoresDisponibles(&this->planificador);
		t_list* trabajo = this->servicioDeCaptura->pokemonesDisponibles(this->servicioDeCaptura);

		this->asignarTareasDeCaptura(this, trabajo, entrenadoresDisponibles);
		sem_post(&this->semaforoEjecucionHabilitadaCapturas);
	}
	sem_post(&this->semaforoFinDeTrabajoCapturas);
}

void planificadorDeCortoPlazo(ServicioDePlanificacion* this) {
	log_debug(this->logger, "Hilo de Planificador de corto plazo creado.");
	while (!this->finDeTrabajoCortoPlazo) {
		sem_wait(&this->semaforoEjecucionHabilitadaCortoPlazo);
		sem_wait(&semaforoReady);

		if (list_is_empty(this->planificador.colas->colaReady)) {
			log_error(this->logger, "Se quizo planificar con una cola de ready vacía!!! (Si estás corriendo los tests es esperable.)");
			sem_post(&this->semaforoEjecucionHabilitadaCortoPlazo);
			continue;
		}

		if (this->finDeTrabajoCortoPlazo) {
			log_debug(this->logger, "Se terminó el hilo de Planificador de corto plazo");
			break;
		}
		log_debug(this->logger, "Iniciando planificación de capturas.");

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

		sem_post(&this->semaforoEjecucionHabilitadaCortoPlazo);
	}
	sem_post(&this->semaforoFinDeTrabajoCortoPlazo);
}

void planificadorDeDeadlocks(ServicioDePlanificacion* this) {
	log_debug(this->logger, "Hilo de Planificador de deadlocks creado.");

	while (!this->finDeTrabajoDeadlock) {
		sem_wait(&this->semaforoEjecucionHabilitadaDeadlock);
		// Debería recibir post cuando llega un Caught, un hilo pasa a BLOCK o EXIT.
		sem_wait(&semaforoDeadlock);
		if (this->finDeTrabajoDeadlock) {
			log_debug(this->logger, "Se terminó el hilo de Planificador de deadlocks");
			break;
		}

		if (list_is_empty(this->planificador.colas->colaBlocked)) {
			log_error(this->logger, "Se quizo tratar deadlocks con una lista de blocked vacía!!! (Si estás corriendo los tests es esperable.)");
			sem_post(&this->semaforoEjecucionHabilitadaDeadlock);
			continue;
		}

		for (int a = 0; a < list_size(this->planificador.colas->colaBlocked); a++) {
			HiloEntrenadorPlanificable* hilo = (HiloEntrenadorPlanificable*) list_get(this->planificador.colas->colaBlocked, a);
			if (hilo->entrenador->objetivoCompletado(hilo->entrenador)) {
				this->planificador.moverACola(&this->planificador, hilo, EXIT, "Estaba en espera de un evento para finalizar, y llegó.");
			}
		}
		if (this->evaluarEstadoPosibleDeadlock(this)) {
			log_debug(this->logger, "Se detectó un estado de posible deadlock.");
			t_list* listaDeBloqueados = this->planificador.colas->colaBlocked;
			t_list* listaDeIntercambios = this->servicioDeResolucionDeDeadlocks->procesarDeadlock(this->servicioDeResolucionDeDeadlocks, listaDeBloqueados);
			this->asignarIntercambios(this, listaDeIntercambios);
		}
		sem_post(&this->semaforoEjecucionHabilitadaDeadlock);
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

static EntrenadorConPokemon* entrenadorOptimo(ServicioDePlanificacion* this, t_list* pokemones, t_list* entrenadores) {
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
	log_info(this->logger, "Asignando tareas de captura");
	int iteracionesPosibles = MIN(list_size(listaPokemon), list_size(entrenadoresDisponibles));
	for (int i = 0; i < iteracionesPosibles; i++) {
		EntrenadorConPokemon* ganador = this->entrenadorOptimo(this, listaPokemon, entrenadoresDisponibles);
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
			this->planificador.moverACola(&this->planificador, hiloElegido, READY, "Se le asignó una tarea de captura.");
			bool pokemon_igual_a(void* elem) {
				PokemonAtrapable * poke = (PokemonAtrapable*) elem;
				return string_equals_ignore_case(poke->especie, pokemon->especie);
			}
			list_remove_by_condition(listaPokemon, pokemon_igual_a);
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
		sem_wait(&semaforoContadorPokemon);
		sem_wait(&semaforoContadorEntrenadoresDisponibles);
	}
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

		this->planificador.moverACola(&this->planificador, intercambio->entrenadorQueSeMueve, READY, "Se le asignó una tarea de intercambio.");

		this->servicioDeMetricas->registrarIntercambio(this->servicioDeMetricas, intercambio->entrenadorQueSeMueve->entrenador->id);
		free(intercambio->pokemonQueObtieneElQueEspera);
		free(intercambio->pokemonQueObtieneElQueSeMueve);
		free(intercambio);
	}
}

void definirYCambiarEstado(ServicioDePlanificacion* this, UnidadPlanificable* hilo) {
	if (hilo->tareaAsignada == NULL) {
		if (hilo->entrenador->estaEsperandoAlgo) {
			this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un evento.");
			return;
		}
		if (!hilo->entrenador->objetivoCompletado(hilo->entrenador) && !hilo->entrenador->puedeAtraparPokemones(hilo->entrenador)) {
			this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un intercambio.");
			return;
		}
	}
	if (hilo->entrenador->objetivoCompletado(hilo->entrenador)) {
		this->planificador.moverACola(&this->planificador, hilo, EXIT, "Completó su objetivo personal.");
		return;
	}
	if (hilo->tareaAsignada != NULL) {
		if (hilo->tareaAsignada->cantidadInstruccionesEjecutadas(hilo->tareaAsignada) < hilo->tareaAsignada->totalInstrucciones) {
			this->planificador.moverACola(&this->planificador, hilo, READY, "Terminó una ráfaga y aún le quedan ciclos pendientes.");
			return;
		}
	}
	if (hilo->entrenador->puedeAtraparPokemones(hilo->entrenador)) {
		this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un evento.");
		return;
	}

	if (hilo->tareaAsignada->cantidadInstruccionesEjecutadas(hilo->tareaAsignada) > hilo->tareaAsignada->totalInstrucciones) {
		log_error(this->logger, "El entrenador '%s' ejecutó más de lo que debía", hilo->entrenador->id);
		return;
	}
	this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Terminó en un estado block. Hay algo RANCIO!!!!!");
}

bool teamFinalizado(ServicioDePlanificacion* this) { // TODO: se puede ir
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

void destruir(ServicioDePlanificacion* this) {
	log_debug(this->logger, "Se procede a destruir al servicio de planificacion");
	this->finDeTrabajoCapturas = true;
	this->finDeTrabajoCortoPlazo = true;
	this->finDeTrabajoDeadlock = true;

	// Posteamos los semaforos con el flag -> va a entrar y morir.
	sem_post(&semaforoContadorEntrenadoresDisponibles);
	sem_post(&semaforoContadorPokemon);
	sem_post(&semaforoReady);
	sem_post(&semaforoDeadlock);

	sem_post(&this->semaforoEjecucionHabilitadaCapturas);
	sem_wait(&this->semaforoFinDeTrabajoCapturas);

	sem_post(&this->semaforoEjecucionHabilitadaCortoPlazo);
	sem_wait(&this->semaforoFinDeTrabajoCortoPlazo);

	sem_post(&this->semaforoEjecucionHabilitadaDeadlock);
	sem_wait(&this->semaforoFinDeTrabajoDeadlock);

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
	servicio->planificadorDeCortoPlazo = &planificadorDeCortoPlazo;
	servicio->planificadorDeDeadlocks = &planificadorDeDeadlocks;
	servicio->asignarTareasDeCaptura = &asignarTareasDeCaptura;
	servicio->definirYCambiarEstado = &definirYCambiarEstado;
	servicio->teamFinalizado = &teamFinalizado;
	servicio->evaluarEstadoPosibleDeadlock = &evaluarEstadoPosibleDeadlock;
	servicio->entrenadorOptimo = &entrenadorOptimo;
	servicio->destruir = &destruir;

	crearHilo((void *(*)(void *)) servicio->planificadorDeCapturas, servicio);
	crearHilo((void *(*)(void *)) servicio->planificadorDeCortoPlazo, servicio);
	crearHilo((void *(*)(void *)) servicio->planificadorDeDeadlocks, servicio);

	return servicio;
}

const struct ServicioDePlanificacionClass ServicioDePlanificacionConstructor = { .new = &new };

t_list * convertirAUnidadesPlanificables(Equipo equipo) {
	return list_map(equipo, (void *(*)(void *)) HiloEntrenadorPlanificableConstructor.new);
}

void destruirUnidadPlanificable(UnidadPlanificable * unidadPlanificable) {
	unidadPlanificable->destruir(unidadPlanificable);
}
