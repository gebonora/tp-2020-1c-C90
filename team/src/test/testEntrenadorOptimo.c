

#include "test/TestDeIntegracion.h"

void testEntrenadorOptimo(){

t_log * testEntrenadorOptimoLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestEntrenadorOptimo", 1, LOG_LEVEL_INFO);
// crear entrenadores y meterlos en una lista

	Entrenador * entrenadora = EntrenadorConstructor.new("1|2", "A|B", "A|C|C");
	Entrenador * entrenadora2 = EntrenadorConstructor.new("4|4", "A|B", "A|B");


	HiloEntrenadorPlanificable * unhiloEntrenadorPlanificable = HiloEntrenadorPlanificableConstructor.new(entrenadora);
	HiloEntrenadorPlanificable * unhiloEntrenadorPlanificable2 = HiloEntrenadorPlanificableConstructor.new(entrenadora2);

	t_list* unos_entrenadores = list_create();
	list_add(unos_entrenadores,unhiloEntrenadorPlanificable);
	list_add(unos_entrenadores,unhiloEntrenadorPlanificable2);

	log_info(testEntrenadorOptimoLogger,"entrenadores agregados a la lista");
// crear pokemones y meterlos en una lista

	PokemonAtrapable * pokemonPika = PokemonAtrapableConstructor.new("PIKACHU", "1|3");

	t_list* pokemones = list_create();
	list_add(pokemones,pokemonPika);

	log_info(testEntrenadorOptimoLogger,"pokemon agregado a su lista");
// levantar un servicio de planificacion

	ServicioDeMetricas* metricasTest1 = ServicioDeMetricasConstructor.new();
	log_info(testEntrenadorOptimoLogger, "metricas creado");
	ServicioDeResolucionDeDeadlocks* deadlocksTest1 = ServicioDeResolucionDeDeadlocksConstructor.new(metricasTest1);
	log_info(testEntrenadorOptimoLogger,"deadlock creado");
	ServicioDePlanificacion * servicio1 = ServicioDePlanificacionConstructor.new(metricasTest1, deadlocksTest1);
	log_info(testEntrenadorOptimoLogger,"serv planificacion creado");
// loggear el entrenado elegio en cada caso


	log_info(testEntrenadorOptimoLogger,"test 1: entrenador optimo");
	assert(string_equals(entrenadora->id,
			((servicio1->entrenadorOptimo(servicio1, pokemones, unos_entrenadores))->entrenadore->entrenador->id)));
	log_info(testEntrenadorOptimoLogger,"test 1: entrenador optimo -> FINALIZADO");

	log_info(testEntrenadorOptimoLogger,"test 1: pokemon del entrandor optimo");
	assert(string_equals(pokemonPika->especie,
			(servicio1->entrenadorOptimo(servicio1, pokemones, unos_entrenadores))->pokemon->especie));

// destruir lo que sea necesario

    list_destroy_and_destroy_elements(unos_entrenadores, (void (*)(void *)) unhiloEntrenadorPlanificable->destruir);
    entrenadora->destruir(entrenadora);
    entrenadora2->destruir(entrenadora2);

    list_destroy_and_destroy_elements(pokemones, (void (*)(void *)) pokemonPika->destruir);

	log_destroy(testEntrenadorOptimoLogger);
}
