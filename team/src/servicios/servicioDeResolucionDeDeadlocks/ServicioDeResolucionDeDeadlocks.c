/*
 * ServicioDeResolucionDeDeadlocks.c
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "servicios/servicioDeResolucionDeDeadlocks/ServicioDeResolucionDeDeadlocks.h"

bool detectarDeadlock(ServicioDeResolucionDeDeadlocks * this, t_list* entrenadoresBloqueados) {
	/* Me llegan los entrenadores llenos y bloqueados desde el planificador.
	 * 		Va a haber uno o mas deadlocks.
	 * 		Quiero llamar al servicio de metricas 1 vez cuando detecto el primer pantallazo de deadlocks, ya que corro el algorimo varias veces
	 * 		pero el numero de deadlocks que hubo a nivel sistema es fijo.
	 *
	 * 		Adentro de esta funcion llamo a resolverDeadlock, que retorna una serie de tareas para resolver los deadlocks.
	 *
	 *
	 *
	 *
	 */

	bool esProcesable(void* elem) { // Capaz sacarla afuera.
		Entrenador* entrenador = (Entrenador*) elem;
		return !entrenador->estaIntercambiando;
	}
	t_list* entrenadoresFiltrados = list_filter(entrenadoresBloqueados, esProcesable); // TODO: Esta copia hay que liberarla?

	t_list* listaDeListas = list_create();

	for (int a = 0; a < list_size(entrenadoresFiltrados); a++) {
		Entrenador* entrenadorActual = (Entrenador*) list_get(entrenadoresFiltrados, a);
		printf("ENTRENADOR: %s\n", entrenadorActual->id);
		t_list* listaCapturasEntrenadorActual = obtenerListaDePokemon(entrenadorActual->pokemonesCapturados);
		t_list* listaObjetivosEntrenadorActual = obtenerListaDePokemon(entrenadorActual->pokemonesObjetivo);

		// Filtrar las listas para sacar del procesamiento a los pokemon que tengo capturas y necesito, así que no se toquen.
		puts("CAPTURAS:");
		imprimirListaStrings(listaCapturasEntrenadorActual);
		puts("OBJETIVOS:");
		imprimirListaStrings(listaObjetivosEntrenadorActual);

		t_list* capturasFiltradas = restarPrimerListaASegunda(listaCapturasEntrenadorActual, listaObjetivosEntrenadorActual);
		t_list* objetivosFiltrados = restarPrimerListaASegunda(listaObjetivosEntrenadorActual, listaCapturasEntrenadorActual);

		puts("CAPTURAS FILTRADAS:");
		imprimirListaStrings(capturasFiltradas);
		puts("OBJETIVOS FILTRADOS:");
		imprimirListaStrings(objetivosFiltrados);

		for (int b = 0; b < list_size(entrenadoresFiltrados); b++) {
			if (a != b) { // Ignorarse a si mismo
				Entrenador* entrenadorIterando = (Entrenador*) list_get(entrenadoresFiltrados, b);
				t_list* listaCapturasEntrenadorIterando = obtenerListaDePokemon(entrenadorIterando->pokemonesCapturados);
				t_list* listaObjetivosEntrenadorIterando = obtenerListaDePokemon(entrenadorIterando->pokemonesObjetivo);

			}
		}

	}

	return true;
}

// Funciones por ahora estáticas.

bool hayDependencias(t_list* listaCapturas, t_list* listaObjetivos) {
	// Evalua si en la listaCapturas hay un elemento de listaObjetivos;
	for (int a = 0; a < list_size(listaObjetivos); a++) {
		char * elemActual = (char*) list_get(listaObjetivos, a);
		for (int b = 0; b < list_size(listaCapturas); b++) {
			char* elemIterado = (char*) list_get(listaCapturas, b);
			if (string_equals_ignore_case(elemActual, elemIterado)) {
				return true;
			}
		}
	}
	return false;
}

t_list* restarPrimerListaASegunda(t_list* lista1, t_list* lista2) {
	t_list* listaA = copiarListaYElementos(lista1);
	t_list* listaB = copiarListaYElementos(lista2);
	void recursion() {
		for (int a = 0; a < list_size(listaA); a++) {
			char * elemActual = (char*) list_get(listaA, a);
			for (int b = 0; b < list_size(listaB); b++) {
				char* elemIterado = (char*) list_get(listaB, b);
				if (string_equals_ignore_case(elemActual, elemIterado)) {
					list_remove_and_destroy_element(listaA, a, free);
					list_remove_and_destroy_element(listaB, b, free);
					recursion();
				}
			}
		}
	}
	recursion();
	list_destroy_and_destroy_elements(listaB, free);
	return listaA;
}

t_list* obtenerListaDePokemon(ContadorPokemones contador) {
// Crea una lista, los elementos son copia del original, para evitar posibles conflictos
// list_destroy_and_elements cuando se termina de usar la lista.
	t_list* listaDePokemon = list_create();
	void iterar(char* pokemon, void* cantidad) {
		for (int a = 0; a < (int) cantidad; a++) {
			list_add(listaDePokemon, string_duplicate(pokemon));
		}
	}
	dictionary_iterator(contador, iterar);
	return listaDePokemon;
}

void imprimirListaStrings(t_list* lista) { // Por ahora con printf.
	void imprimirElemento(void* elem) {
		char* string = (char*) elem;
		printf("%s\n", string);
	}
	list_iterate(lista, imprimirElemento);
}

t_list* copiarListaYElementos(t_list* lista) {
	t_list* retorno = list_create();
	for (int a = 0; a < list_size(lista); a++) {
		list_add(retorno, string_duplicate((char*) list_get(lista, a)));
	}
	return retorno;
}

// End funciones por ahora estáticas

static void destruirServicioDeResolucionDeDeadlocks(ServicioDeResolucionDeDeadlocks * this) {
	log_debug(this->logger, "Se procede a destruir al servicio de resolución de deadlocks");
	log_destroy(this->logger);
	free(this);
}

static ServicioDeResolucionDeDeadlocks * new(Mapa mapa, Equipo equipo, ServicioDePlanificacion * servicioDePlanificacion) {
	ServicioDeResolucionDeDeadlocks * servicio = malloc(sizeof(ServicioDeResolucionDeDeadlocks));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeResolucionDeDeadlocks", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_DEBUG);
	servicio->detectarDeadlock = &detectarDeadlock;
	servicio->destruir = &destruirServicioDeResolucionDeDeadlocks;

	return servicio;
}

const struct ServicioDeResolucionDeDeadlocksClass ServicioDeResolucionDeDeadlocksConstructor = { .new = &new };
