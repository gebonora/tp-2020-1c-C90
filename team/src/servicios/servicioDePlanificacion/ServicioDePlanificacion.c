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

		log_warning(this->logger, "ESPERANDO SEM CONTADOR POKEMON CICLO");
		sem_wait(&semaforoContadorPokemon);
		log_warning(this->logger, "ESPERANDO SEM ENTRENADOR CICLO");
		sem_wait(&semaforoContadorEntrenadoresDisponibles);

		if (this->finDeTrabajoCapturas) { // Esto permite al hilo morirse
			log_debug(this->logger, "Se terminó el hilo de Planificador de capturas");
			break;
		}
		log_debug(this->logger, "Iniciando planificación de capturas.");

		if (list_is_empty(this->servicioDeCaptura->pokemonesAtrapables)
				|| (list_is_empty(this->planificador.colas->colaBlocked) && list_is_empty(this->planificador.colas->colaNew))) {
			log_error(this->logger, "Se quizo asignar capturas con listas vacías!!! (Si estás corriendo los tests es esperable.)");
			this->planificador.mostrarLasColas(&this->planificador);
			sem_post(&this->semaforoEjecucionHabilitadaCapturas);
			continue;
		}

		t_list* entrenadoresDisponibles = this->planificador.armarListaEntrenadoresDisponibles(&this->planificador);
		t_list* trabajo = this->servicioDeCaptura->pokemonesDisponibles(this->servicioDeCaptura);
		this->planificador.mostrarLasColas(&this->planificador);

		this->asignarTareasDeCaptura(this, trabajo, entrenadoresDisponibles);
		this->planificador.mostrarLasColas(&this->planificador);

		sem_post(&this->semaforoEjecucionHabilitadaCapturas);
	}
	sem_post(&this->semaforoFinDeTrabajoCapturas);
}

void planificadorDeCortoPlazo(ServicioDePlanificacion* this) {
	log_debug(this->logger, "Hilo de Planificador de corto plazo creado.");
	while (!this->finDeTrabajoCortoPlazo) {
		sem_wait(&this->semaforoEjecucionHabilitadaCortoPlazo);
		sem_wait(&semaforoReady);

		if (this->finDeTrabajoCortoPlazo) {
			log_debug(this->logger, "Se terminó el hilo de Planificador de corto plazo");
			break;
		}

		log_debug(this->logger, "Iniciando planificación de corto plazo.");

		if (list_is_empty(this->planificador.colas->colaReady)) {
			log_error(this->logger, "Se quizo planificar con una cola de ready vacía!!! (Si estás corriendo los tests es esperable.)");
			this->planificador.mostrarLasColas(&this->planificador);
			sem_post(&this->semaforoEjecucionHabilitadaCortoPlazo);
			continue;
		}

		HiloEntrenadorPlanificable* aEjecutar = this->planificador.obtenerProximoAEjecutar(&this->planificador);
		int ciclosAEjecutar = this->planificador.cantidadDeRafagas(&this->planificador, aEjecutar);

		log_info(this->logger, "va a ejecutar %s por %d.", aEjecutar->entrenador->id, ciclosAEjecutar);

		pthread_mutex_lock(&aEjecutar->entrenador->mutex);
		this->planificador.moverACola(&this->planificador, aEjecutar, EXEC, "Ejecutará en el procesador.");

		// chequeamos si es igual al entrenador que se trabajaba antes y si no lo es se registra el context switch
		if (this->ultimoHiloEjecutado != NULL) {
			if (!(string_equals(aEjecutar->entrenador->id, this->ultimoHiloEjecutado->entrenador->id))) {
				this->servicioDeMetricas->registrarCambioDeContexto(this->servicioDeMetricas);
			}
		} else {
			// aca registramos el primer cambio de contexto -> de no tener nada a tener algo en exec
			this->servicioDeMetricas->registrarCambioDeContexto(this->servicioDeMetricas);
			this->ultimoHiloEjecutado = aEjecutar;
		}

		aEjecutar->ejecutarParcialmente(aEjecutar, ciclosAEjecutar);
		this->ultimoHiloEjecutado = aEjecutar;

		for (int a = 0; a < ciclosAEjecutar; a++) {
			this->servicioDeMetricas->registrarCicloRealizadoPorEntrenador(this->servicioDeMetricas, aEjecutar->entrenador->id);
		}
		this->definirYCambiarEstado(this, aEjecutar);
		pthread_mutex_unlock(&aEjecutar->entrenador->mutex);

		this->planificador.mostrarLasColas(&this->planificador);

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

		log_debug(this->logger, "Iniciando planificación de deadlocks.");

		for (int a = 0; a < list_size(this->planificador.colas->colaBlocked); a++) {
			HiloEntrenadorPlanificable* hilo = (HiloEntrenadorPlanificable*) list_get(this->planificador.colas->colaBlocked, a);
			if (hilo->entrenador->objetivoCompletado(hilo->entrenador)) {
				this->planificador.moverACola(&this->planificador, hilo, EXIT, "Estaba en espera de un evento para finalizar, y llegó.");

				// agrego aca contar el cambio de contexto cuando se saca al ultimo entrenador de exec
				if (list_is_empty(this->planificador.colas->colaExec) && list_is_empty(this->planificador.colas->colaReady)
						&& list_is_empty(this->planificador.colas->colaBlocked)) {

					this->servicioDeMetricas->registrarCambioDeContexto(this->servicioDeMetricas);
				}
			}
		}
		if (this->evaluarEstadoPosibleDeadlock(this)) {
			log_debug(this->logger, "Se detectó un estado de posible deadlock.");
			t_list* listaDeBloqueados = this->planificador.colas->colaBlocked;
			t_list* listaDeIntercambios = this->servicioDeResolucionDeDeadlocks->procesarDeadlock(this->servicioDeResolucionDeDeadlocks, listaDeBloqueados);
			this->asignarIntercambios(this, listaDeIntercambios);
		}
		this->planificador.mostrarLasColas(&this->planificador);
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

static EntrenadorConPokemon* entrenadorOptimo(ServicioDePlanificacion* this, t_list* pokemones, t_list* entrenadores) { // TODO: Cambiar para que respete orden.
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

	log_error(this->logger, "tamanio listaPokemons: %d", list_size(listaPokemon));
	bool pokemon_capturable(void* elem) {
		PokemonAtrapable* poke = (PokemonAtrapable*) elem;
		return this->objetivoGlobal.puedeCapturarse(&this->objetivoGlobal, poke->especie);
	}


	t_list* pokemones_capturables = list_filter(listaPokemon, pokemon_capturable);


	// Si la lista está vacía salimos.


	if(list_is_empty(pokemones_capturables)){
		log_error(this->logger, "Saliendo porque se filtró y quedó la lista vacía.");
		list_destroy(listaPokemon);
		list_destroy(pokemones_capturables);
		list_destroy(entrenadoresDisponibles);
		sem_post(&semaforoContadorEntrenadoresDisponibles);
		return;
	}

	for (int a = 0; a < list_size(pokemones_capturables); a++) {
		PokemonAtrapable* pok = list_get(listaPokemon, a);
		log_error(this->logger, "posicion: %d %s", a, pok->especie);
	}

	PokemonAtrapable* pokemonAPlanificar = list_get(pokemones_capturables, 0);
	Coordinate coordenadaPokemon = pokemonAPlanificar->gps->posicionActual(pokemonAPlanificar->gps).coordenada;

	bool masCercano(void* elem1, void* elem2) {
		HiloEntrenadorPlanificable* hilo1 = elem1;
		HiloEntrenadorPlanificable* hilo2 = elem2;
		Coordinate coor1 = hilo1->entrenador->gps->posicionActual(hilo1->entrenador->gps).coordenada;
		Coordinate coor2 = hilo2->entrenador->gps->posicionActual(hilo2->entrenador->gps).coordenada;
		return distanciaEntre(coordenadaPokemon, coor1) <= distanciaEntre(coordenadaPokemon, coor2);
	}

	list_sort(entrenadoresDisponibles, masCercano);

	for (int a = 0; a < list_size(entrenadoresDisponibles); a++) {
		HiloEntrenadorPlanificable* hilda = list_get(entrenadoresDisponibles, a);
		log_error(this->logger, "posicion: %d %s", a, hilda->entrenador->id);
	}

	HiloEntrenadorPlanificable* hiloElegido = list_get(entrenadoresDisponibles, 0);
	log_info(this->logger, "Ya tengo un entrenador optimo, es el %s", hiloElegido->entrenador->id);
	TareaPlanificable* tarea = generarTareaDeCaptura(hiloElegido->entrenador, pokemonAPlanificar->especie, coordenadaPokemon);
	hiloElegido->asignarTarea(hiloElegido, tarea);
	hiloElegido->entrenador->estaEsperandoAlgo = true;
	hiloElegido->infoUltimaEjecucion.seNecesitaNuevaEstimacion = true;
	hiloElegido->infoUltimaEjecucion.rafaga_real_actual = hiloElegido->tareaAsignada->totalInstrucciones;
	pokemonAPlanificar->marcarComoObjetivo(pokemonAPlanificar, hiloElegido->entrenador->id);
	this->objetivoGlobal.restarUnCapturado(&this->objetivoGlobal, pokemonAPlanificar->especie);
	this->planificador.moverACola(&this->planificador, hiloElegido, READY, "Se le asignó una tarea de captura.");
	list_destroy(listaPokemon);
	list_destroy(pokemones_capturables);
	list_destroy(entrenadoresDisponibles);
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
	list_destroy(intercambios);
}

void definirYCambiarEstado(ServicioDePlanificacion* this, UnidadPlanificable* hilo) {
	if (hilo->tareaAsignada == NULL) {
		if (hilo->entrenador->objetivoCompletado(hilo->entrenador)) {
			this->planificador.moverACola(&this->planificador, hilo, EXIT, "Completó su objetivo personal.");
			return;
		} else {
			this->planificador.moverACola(&this->planificador, hilo, BLOCK, "Está en espera de un evento.");
			if (!hilo->entrenador->estaEsperandoAlgo && hilo->entrenador->puedeAtraparPokemones(hilo->entrenador)) {
				sem_post(&semaforoContadorEntrenadoresDisponibles);
				log_warning(this->logger, "Postié el semaforo entrenadores disponibles");
			}
			return;

		}
	} else {
		if (hilo->tareaAsignada->cantidadInstruccionesEjecutadas(hilo->tareaAsignada) < hilo->tareaAsignada->totalInstrucciones) {
			this->planificador.moverACola(&this->planificador, hilo, READY, "Terminó una ráfaga y aún le quedan ciclos pendientes.");
			return;
		}
	}
	log_error(this->logger, "NO SE PUDO ASIGNAR UN ESTADO!!!");
}

bool teamFinalizado(ServicioDePlanificacion* this) { // TODO: se puede ir
	return !list_is_empty(this->planificador.colas->colaExit) && list_is_empty(this->planificador.colas->colaReady) && list_is_empty(this->planificador.colas->colaExec)
			&& list_is_empty(this->planificador.colas->colaBlocked) && list_is_empty(this->planificador.colas->colaNew);
}

bool evaluarEstadoPosibleDeadlock(ServicioDePlanificacion* this) {

	bool entrenadorEnDeadlock(void* elem) {
		HiloEntrenadorPlanificable* hilo = (HiloEntrenadorPlanificable*) elem;
		return !hilo->entrenador->puedeAtraparPokemones(hilo->entrenador) && !hilo->entrenador->objetivoCompletado(hilo->entrenador);
	}
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

static ServicioDePlanificacion * new(ServicioDeMetricas* servicioDeMetricas, ServicioDeResolucionDeDeadlocks* servicioDeadlocks) {
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


