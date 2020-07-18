//
// Created by Alan Zhao on 09/07/2020.
//

#include "test/TestDeIntegracion.h"

void testDeEntrenadores() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestEntrenadores", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando los metodos de los entrenadores");
    Entrenador * entrenador = EntrenadorConstructor.new("1|2", "A|B", "A|C|C");
    Entrenador * entrenador2 = EntrenadorConstructor.new("2|4", "A|B", "A|B");

    assert(entrenador->objetivoCompletado(entrenador) == false);
    assert(entrenador->puedeAtraparPokemones(entrenador) == true);
    assert(entrenador2->objetivoCompletado(entrenador2) == true);
    assert(entrenador2->puedeAtraparPokemones(entrenador2) == false);
    assert((int) dictionary_get(entrenador->pokemonesCapturados, "A") == 1);
    assert((int) dictionary_get(entrenador->pokemonesCapturados, "B") == 1);
    assert((int) dictionary_get(entrenador->pokemonesObjetivo, "A") == 1);
    assert((int) dictionary_get(entrenador->pokemonesObjetivo, "C") == 2);
    assert((int) dictionary_get(entrenador2->pokemonesCapturados, "A") == 1);
    assert((int) dictionary_get(entrenador2->pokemonesCapturados, "B") == 1);
    assert((int) dictionary_get(entrenador2->pokemonesObjetivo, "A") == 1);
    assert((int) dictionary_get(entrenador2->pokemonesObjetivo, "B") == 1);

    log_info(testLogger, "Testeando entrenador sin pokemones capturados");
    Entrenador * entrenador3 = EntrenadorConstructor.new("2|4", "None", "A|B");
    assert((int) dictionary_get(entrenador3->pokemonesCapturados, "A") == 0);
    assert((int) dictionary_get(entrenador3->pokemonesCapturados, "B") == 0);
    assert(dictionary_size(entrenador3->pokemonesCapturados) == 0);
    assert(entrenador3->objetivoCompletado(entrenador3) == false);

    log_info(testLogger, "Testeando que la captura sume al contador del entrenador");
    PokemonAtrapable * pokemonA = PokemonAtrapableConstructor.new("A", "0|1");
    PokemonAtrapable * pokemonB = PokemonAtrapableConstructor.new("B", "1|1");
    CapturaPokemon * capturaPokemonA = CapturaPokemonConstructor.new("", 0, pokemonA);
    CapturaPokemon * capturaPokemonB = CapturaPokemonConstructor.new("", 0, pokemonB);
    entrenador3->registrarCaptura(entrenador3, capturaPokemonA);
    assert((int) dictionary_get(entrenador3->pokemonesCapturados, "A") == 1);
    entrenador3->registrarCaptura(entrenador3, capturaPokemonB);
    assert((int) dictionary_get(entrenador3->pokemonesCapturados, "B") == 1);
    assert(entrenador3->objetivoCompletado(entrenador3) == true);

    log_info(testLogger, "Testeando los metodos del objetivo global");

    Equipo equipito = list_create();
    list_add(equipito, entrenador);
    list_add(equipito, entrenador2);

    ObjetivoGlobal objetivo = ObjetivoGlobalConstructor.new(equipito);

    t_list * especiesNecesarias = objetivo.especiesNecesarias(&objetivo);

    ContabilidadEspecie * contabilidadEspecieA = dictionary_get(objetivo.contabilidadEspecies, "A");
    ContabilidadEspecie * contabilidadEspecieB = dictionary_get(objetivo.contabilidadEspecies, "B");
    ContabilidadEspecie * contabilidadEspecieC = dictionary_get(objetivo.contabilidadEspecies, "C");

    assert(list_size(especiesNecesarias) == 3);
    assert(string_equals(list_get(especiesNecesarias, 0),"A"));
    assert(string_equals(list_get(especiesNecesarias, 1),"B"));
    assert(string_equals(list_get(especiesNecesarias, 2),"C"));
    assert(objetivo.puedeCapturarse(&objetivo, "A") == false);
    assert(objetivo.puedeCapturarse(&objetivo, "B") == false);
    assert(objetivo.puedeCapturarse(&objetivo, "C") == true);
    assert(objetivo.puedeCapturarse(&objetivo, "X") == false);
    assert(contabilidadEspecieA->necesarios == 2);
    assert(contabilidadEspecieA->capturados == 2);
    assert(contabilidadEspecieB->necesarios == 1);
    assert(contabilidadEspecieB->capturados == 2);
    assert(contabilidadEspecieC->necesarios == 2);
    assert(contabilidadEspecieC->capturados == 0);

    entrenador3->destruir(entrenador3);
    capturaPokemonA->destruir(capturaPokemonA);
    capturaPokemonB->destruir(capturaPokemonB);
    list_destroy(especiesNecesarias);
    destruirEquipo(equipito);
    objetivo.destruirObjetivoGlobal(&objetivo);
    log_destroy(testLogger);
}