//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeEventos() {
	t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestEventos", 1, LOG_LEVEL_INFO);
	log_info(testLogger, "Testeando los eventos del sistema");

	Mapa mapa = MapaConstructor.new();
	Entrenador * entrenador = EntrenadorConstructor.new("1|2", "", "");
	registrarEnMapaPosicionEntrenador(&mapa, entrenador);
	Equipo equipo = list_create();
	list_add(equipo, entrenador);

	ServicioDePlanificacion * servicioPlanificacion = ServicioDePlanificacionConstructor.new();
	ServicioDeCaptura * servicioDeCapturaTest = ServicioDeCapturaConstructor.new(mapa, servicioPlanificacion);

    RegistradorDeEventos * registrador = RegistradorDeEventosConstructor.new();
    ManejadorDeEventos* manejadorDeEventosTest = ManejadorDeEventosConstructor.new(servicioDeCapturaTest, registrador);

    //PokemonAtrapable * pokemonAtrapable = PokemonAtrapableConstructor.new("Pikachu", "1|2");

    Coordinate posicion = (Coordinate) {.pos_x = 1, .pos_y = 2};
    PokemonAtrapable * pokemonAtrapable = servicioDeCapturaTest->altaDePokemon(servicioDeCapturaTest, "Pikachu", posicion);

	CapturaPokemon* pokemon = CapturaPokemonConstructor.new(entrenador, 200, pokemonAtrapable);

	Caught* caughtOK = create_caught_pokemon(OK);
	Caught* caughtOK2 = create_caught_pokemon(OK);
	Caught* caughtFAIL = create_caught_pokemon(FAIL);

	Pokemon* charmander = create_pokemon("Charmander", 1, 2);

	Localized* localizedCharmander = malloc(sizeof(Localized));
	localizedCharmander->pokemon = create_pokemon("Charmander", 1, 3);
	localizedCharmander->coordinates_quantity = 1;

    registrador->registrarGetEnEspera(registrador, "Charmander", 100);
    registrador->registrarCatchEnEspera(registrador, pokemon);

    assert(list_size(registrador->listaCatchEnEspera->lista) == 1);
    assert(list_size(registrador->listaGetEnEspera->lista) == 1);

	manejadorDeEventosTest->procesarCaughtRecibido(manejadorDeEventosTest, caughtOK, 200);
	manejadorDeEventosTest->procesarCaughtRecibido(manejadorDeEventosTest, caughtFAIL, 404);
	manejadorDeEventosTest->procesarCaughtRecibido(manejadorDeEventosTest, caughtOK2, 405);

	manejadorDeEventosTest->procesarAppearedRecibido(manejadorDeEventosTest, charmander, 1);
	manejadorDeEventosTest->procesarLocalizedRecibido(manejadorDeEventosTest, localizedCharmander, 100);

	servicioDeCapturaTest->destruir(servicioDeCapturaTest);
	servicioPlanificacion->destruir(servicioPlanificacion);
	mapa.destruir(&mapa);
	destruirEquipo(equipo);
    pokemon->destruir(pokemon); //TODO: Definir bien quien se tiene que encargar de liberar esta estructura.
	manejadorDeEventosTest->destruir(manejadorDeEventosTest);
    registrador->destruir(registrador);
	log_destroy(testLogger);
}
