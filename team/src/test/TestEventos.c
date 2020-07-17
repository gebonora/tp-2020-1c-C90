//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeEventos() {
	t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestEventos", 1, LOG_LEVEL_INFO);
	log_info(testLogger, "Testeando los eventos del sistema");

	Mapa mapa = MapaConstructor.new();
	Entrenador * entrenador = EntrenadorConstructor.new("1|2", "", "");
	free(entrenador->id);
	entrenador->id = string_duplicate("EntrenadorPiola");
	registrarEnMapaPosicionEntrenador(&mapa, entrenador);
	Equipo equipo = list_create();
	list_add(equipo, entrenador);

	ServicioDePlanificacion * servicioPlanificacion = ServicioDePlanificacionConstructor.new();
	ServicioDeCaptura * servicioDeCapturaTest = ServicioDeCapturaConstructor.new(mapa, equipo, servicioPlanificacion);

	ManejadorDeEventos* manejadorDeEventosTest = ManejadorDeEventosConstructor.new(servicioDeCapturaTest);

	PokemonAtrapable * pokemonAtrapable = PokemonAtrapableConstructor.new("Pikachu", "1|2");
	CapturaPokemon* pokemon = CapturaPokemonConstructor.new("EntrenadorPiola", 200, pokemonAtrapable);

	Caught* caughtOK = create_caught_pokemon(OK);
	Caught* caughtOK2 = create_caught_pokemon(OK);
	Caught* caughtFAIL = create_caught_pokemon(FAIL);

	Pokemon* charmander = create_pokemon("Charmander", 1, 2);

	Localized* localizedCharmander = malloc(sizeof(Localized));
	localizedCharmander->pokemon = create_pokemon("Charmander", 1, 3);
	localizedCharmander->coordinates_quantity = 1;

	MensajeGet* mensajeGet = malloc(sizeof(MensajeGet));
	mensajeGet->nombrePokemon = string_from_format("Charmander");
	mensajeGet->idCorrelatividad = 100;

	manejadorDeEventosTest->registrarGetEnEspera(manejadorDeEventosTest, mensajeGet);
	manejadorDeEventosTest->registrarCatchEnEspera(manejadorDeEventosTest, pokemon);

	manejadorDeEventosTest->procesarCaughtRecibido(manejadorDeEventosTest, caughtOK, 200);
	manejadorDeEventosTest->procesarCaughtRecibido(manejadorDeEventosTest, caughtFAIL, 404);
	manejadorDeEventosTest->procesarCaughtRecibido(manejadorDeEventosTest, caughtOK2, 405);

	manejadorDeEventosTest->procesarAppearedRecibido(manejadorDeEventosTest, charmander, 1);
	manejadorDeEventosTest->procesarLocalizedRecibido(manejadorDeEventosTest, localizedCharmander, 100);

	servicioDeCapturaTest->destruir(servicioDeCapturaTest);
	servicioPlanificacion->destruir(servicioPlanificacion);
	mapa.destruir(&mapa);
	destruirEquipo(equipo);
	pokemon->destruir(pokemon);
	manejadorDeEventosTest->destruir(manejadorDeEventosTest);

	log_destroy(testLogger);

}
