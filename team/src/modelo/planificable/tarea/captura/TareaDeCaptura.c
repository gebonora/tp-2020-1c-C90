//
// Created by Alan Zhao on 07/07/2020.
//

#include "modelo/planificable/tarea/captura/TareaDeCaptura.h"

TareaPlanificable * generarTareaDeCaptura(Entrenador * entrenador, char * pokemon, Coordinate posicionPokemon) {
	Posicion posicionOrigen = entrenador->posicion(entrenador);

	if (posicionOrigen.valida) {
		Camino camino = generarCamino(posicionOrigen.coordenada, posicionPokemon);
		Instrucciones instrucciones = list_create();

		for (int i = 0; i < list_size(camino); i++) {
			Coordinate * punteroCoordenada = list_get(camino, i);
			t_list * args = list_create();
			list_add(args, create_coordinate(punteroCoordenada->pos_x, punteroCoordenada->pos_y));

			Instruccion * instruccion = malloc(sizeof(Instruccion));

			instruccion->posicion = i;
			instruccion->funcion = (void (*)(void *, ...)) entrenador->mover;
			instruccion->descripcion = string_from_format("Mover entrenador a (%d,%d)", punteroCoordenada->pos_x, punteroCoordenada->pos_y);
			instruccion->argumentos = args;

			list_add(instrucciones, instruccion);
		}

		t_list * argumentosCaptura = list_create();
		list_add(argumentosCaptura, string_duplicate(pokemon));
		list_add(argumentosCaptura, convertirAPunteroDeCoordenada(posicionPokemon));

		Instruccion * instruccionFinal = crearInstruccion(list_size(instrucciones), (void (*)(void *, ...)) iniciarCaptura,
				string_from_format("Intento de captura de %s en (%d,%d)", pokemon, posicionPokemon.pos_x, posicionPokemon.pos_y), argumentosCaptura);
		list_add(instrucciones, instruccionFinal);

		TareaPlanificable * tareaDeCaptura = TareaPlanificableConstructor.new(instrucciones);
		list_destroy_and_destroy_elements(camino, free);

		return tareaDeCaptura;
	}

	log_error(INTERNAL_LOGGER, "No es posible generar una tarea de captura para %s ya que no tiene una posicion valida.", entrenador->id);

	return NULL;
}

void iniciarCaptura(Entrenador * entrenador, char * especie, Coordinate * posicionPokemon) {
	char * posicionInicial = separarPunteroCoordenada(posicionPokemon, "|");

	PokemonAtrapable * pokemonAtrapable = servicioDeCapturaProcesoTeam->obtenerPokemonAtrapable(servicioDeCapturaProcesoTeam, especie,
			convertirACoordenada(posicionPokemon));

	if (pokemonAtrapable == NULL) {
		log_error(entrenador->logger, "Se intentó capturar a un pokemon que ya no está presente");
		free(posicionInicial);
		return;
	}

	log_info(MANDATORY_LOGGER, "El entrenador %s procede a intentar capturar un %s en (%d,%d)", entrenador->id, especie, posicionPokemon->pos_x, posicionPokemon->pos_y);

	CapturaPokemon * capturaPokemon = CapturaPokemonConstructor.new(entrenador, 0, pokemonAtrapable);

	RespuestaBroker respuestaBroker = clienteBrokerV2ProcesoTeam->enviarCatch(clienteBrokerV2ProcesoTeam, especie, posicionPokemon->pos_x, posicionPokemon->pos_y);

	if (respuestaBroker.esValida) {
		capturaPokemon->idCorrelatividad = respuestaBroker.idCorrelatividad;
		registradorDeEventosProcesoTeam->registrarCatchEnEspera(registradorDeEventosProcesoTeam, capturaPokemon);
		entrenador->estaEsperandoAlgo = true;
	} else {
		servicioDeCapturaProcesoTeam->registrarCapturaExitosa(servicioDeCapturaProcesoTeam, capturaPokemon);
		entrenador->estaEsperandoAlgo = false;
	}
	free(posicionInicial);
}
