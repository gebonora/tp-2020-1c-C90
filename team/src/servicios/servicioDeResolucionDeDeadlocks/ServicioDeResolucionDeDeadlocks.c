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
	t_list* entrenadoresFiltrados = list_filter(entrenadoresBloqueados, esProcesable); // TODO: Esta copia hay que liberarla? SI pero no los elementos.

	t_list* listaDeDepencias = list_create();

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

		t_list* objetivosFiltrados = restarPrimerListaASegunda(listaObjetivosEntrenadorActual, listaCapturasEntrenadorActual);

		Dependencias* dependenciasEntrenadorActual = malloc(sizeof(Dependencias));
		dependenciasEntrenadorActual->nombreEntrenador = string_duplicate(entrenadorActual->id);
		dependenciasEntrenadorActual->listaDependencias = list_create();

		puts("OBJETIVOS FILTRADOS:");
		imprimirListaStrings(objetivosFiltrados);

		for (int b = 0; b < list_size(entrenadoresFiltrados); b++) {
			if (a != b) { // Ignorarse a si mismo. De todas formas no debería tener dependecias con sí mismo, porque se filtraron.
				Entrenador* entrenadorIterando = (Entrenador*) list_get(entrenadoresFiltrados, b);
				t_list* listaCapturasEntrenadorIterando = obtenerListaDePokemon(entrenadorIterando->pokemonesCapturados);
				t_list* listaObjetivosEntrenadorIterando = obtenerListaDePokemon(entrenadorIterando->pokemonesObjetivo);
				t_list* capturasFiltradasEntrenadorIterando = restarPrimerListaASegunda(listaCapturasEntrenadorIterando, listaObjetivosEntrenadorIterando);

				if (hayDependencias(capturasFiltradasEntrenadorIterando, objetivosFiltrados)) {
					list_add(dependenciasEntrenadorActual->listaDependencias, string_duplicate(entrenadorIterando->id));
				}

				list_destroy_and_destroy_elements(listaCapturasEntrenadorIterando, free);
				list_destroy_and_destroy_elements(listaObjetivosEntrenadorIterando, free);
				list_destroy_and_destroy_elements(capturasFiltradasEntrenadorIterando, free);
			}
		}
		list_destroy_and_destroy_elements(listaCapturasEntrenadorActual, free);
		list_destroy_and_destroy_elements(listaObjetivosEntrenadorActual, free);
		list_destroy_and_destroy_elements(objetivosFiltrados, free);
		if (list_size(dependenciasEntrenadorActual->listaDependencias)) {
			list_add(listaDeDepencias, dependenciasEntrenadorActual);
		} else {
			destruirDependencia(dependenciasEntrenadorActual);
		}

	}

	// tengo en lista de lista una entrada por entrenador con sus dependencias (representadas como string nombreEntrenador).
	// Para resolucion me voy a basar en esta lista de listas. Capaz sacar lo que está hasta ahora en otra funcion, que devuelva la lista de listas,
	// Voy a tener un flag para que la primera vez que se llame al servicio, se haga la detección completa y printee (o servicioMetricas) cuantos deadlocks hay
	// y los entrenadores involucrados. Luego se pasa solo a resolucion. Queremos que el servicioDeadlock tenga un solo punto de entrada desde el planificador.
	// Y devuelva una serie de tareas de intercambio que pueden ejecutarse en paralelo.

	// NOTA IMPORTANTE: asumimos que un entrenador no va ser iniciado desde config con un pokemon que no necesita el equipo.

	imprimirListaDeDependencias(listaDeDepencias);
	if (1) {	// Flag aca
		detectarEnDetalleYLogear(listaDeDepencias);
	}

	list_destroy_and_destroy_elements(listaDeDepencias, destruirDependencia);
	list_destroy(entrenadoresFiltrados);
	return true;
}

// Funciones por ahora estáticas.

void detectarEnDetalleYLogear(t_list* listaDeDepencias) {
	// listaDeDependencias no se toca!!
	t_list* deadlocksTotales = list_create(); // es lista de listas
	puts("caca");

	for (int a = 0; a < list_size(listaDeDepencias); a++) {
		Dependencias* dependenciaActual = (Dependencias*) list_get(listaDeDepencias, a);
		bool flag = false;
		t_list* unDeadlock = list_create();

		for (int b = 0; b < list_size(listaDeDepencias); b++) {
			Dependencias* dependenciaIterada = (Dependencias*) list_get(listaDeDepencias, b);
			if (hayDependenciaEnComun(dependenciaActual, dependenciaIterada)) {
				agregarCopiaDeElementosAListaSinRepetir(unDeadlock, dependenciaIterada->listaDependencias);
			}
		}
		list_add(deadlocksTotales, unDeadlock);
	}
	// Deberia tener en deadlocks una entrada con una lista maxima, quiero sacarle los repetidos a esa lista y estaria ok. Con 1 lista por conjunto de procesos en deadlock.
	puts("DEADLOCKS SIN PROCESAR:");
	imprimirListaDeListas(deadlocksTotales);
	t_list* deadlocks = eliminarListasRepetidas(deadlocksTotales);
	puts("DEADLOCKS PROCESADOS:");

	imprimirListaDeListas(deadlocks);

	list_destroy_and_destroy_elements(deadlocksTotales, destruirListaDeStrings);
	list_destroy(deadlocks);
}

void destruirDependencia(void * elem) {
	Dependencias* dependencia = (Dependencias*) elem;
	free(dependencia->nombreEntrenador);
	list_destroy_and_destroy_elements(dependencia->listaDependencias, free);
	free(dependencia);
}

void destruirListaDeStrings(void* elem) {
	t_list* lista = (t_list*) elem;
	list_destroy_and_destroy_elements(lista, free);
}

t_list* eliminarListasRepetidas(t_list* listaDeListas) {
	t_list* listaConMismosElementos = list_duplicate(listaDeListas);
	for (int a = 0; a < list_size(listaConMismosElementos); a++) {
		t_list* listaActual = (t_list*) list_get(listaConMismosElementos, a);
		for (int b = 0; b < list_size(listaConMismosElementos); b++) {
			if (a != b) {
				t_list* listaIterada = (t_list*) list_get(listaConMismosElementos, b);
				if (hayElementoEnComun(listaActual, listaIterada)) {
					list_remove(listaConMismosElementos, a);
				}
			}
		}
	}
	return listaConMismosElementos;
}

void imprimirListaDeListas(t_list* listaDeListas) {
	puts("IMPRIENDO LISTA DE LISTAS:");
	printf("TAMANIO LISTA DE LISTAS: %d\n", list_size(listaDeListas));
	for (int a = 0; a < list_size(listaDeListas); a++) {
		t_list* listaActual = (t_list*) list_get(listaDeListas, a);
		printf("LISTA NUMERO:: %d, TAMANIO: %d\n", a, list_size(listaActual));
		imprimirListaStrings(listaActual);
	}
}

void imprimirListaDeDependencias(t_list* listaDeDependencias) {
	puts("IMPRIMIENDO LISTA DE DEPENDENCIAS:");
	printf("TAMANIO LISTA DE DEPENDENCIAS: %d\n", list_size(listaDeDependencias));
	for (int a = 0; a < list_size(listaDeDependencias); a++) {
		Dependencias* dependencia = (Dependencias*) list_get(listaDeDependencias, a);
		imprimirDependencias(dependencia);
	}

}

void imprimirDependencias(Dependencias* dependencias) {
	printf("IMPRIENDO DEPENDENCIAS DE: %s\n", dependencias->nombreEntrenador);
	imprimirListaStrings(dependencias->listaDependencias);
}

bool hayElementoEnComun(t_list* lista1, t_list* lista2) {
	for (int a = 0; a < list_size(lista1); a++) {
		char* elem = list_get(lista1, a);
		if (perteneceALista(lista2, elem))
			return true;
	}
	return false;
}

bool hayDependenciaEnComun(Dependencias* dependencia1, Dependencias* dependencia2) {
	t_list* lista1 = copiarListaYElementos(dependencia1->listaDependencias);
	list_add(lista1, string_duplicate(dependencia1->nombreEntrenador));
	t_list* lista2 = copiarListaYElementos(dependencia2->listaDependencias);
	list_add(lista2, string_duplicate(dependencia2->nombreEntrenador));
	bool resultado = hayElementoEnComun(lista1, lista2);
	list_destroy_and_destroy_elements(lista1, free);
	list_destroy_and_destroy_elements(lista2, free);
	return resultado;
}

void agregarCopiaDeElementosAListaSinRepetir(t_list* listaQueRecibe, t_list* listaQueDa) {
	for (int a = 0; a < list_size(listaQueDa); a++) {
		agregarCopiaSinRepetir(listaQueRecibe, (char*) list_get(listaQueDa, a));
	}
}

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

void agregarCopiaSinRepetir(t_list* lista, char* elem) {
	if (perteneceALista(lista, elem))
		return;
	else {
		list_add(lista, string_duplicate(elem));
	}
}

bool perteneceALista(t_list* lista, char* elem) {
	bool esIgual(void* string) {
		return string_equals_ignore_case(elem, (char*) string);
	}
	return list_any_satisfy(lista, esIgual);
}

// End funciones por ahora estáticas

static void destruirServicioDeResolucionDeDeadlocks(ServicioDeResolucionDeDeadlocks * this) {
	log_debug(this->logger, "Se procede a destruir al servicio de resolución de deadlocks");
	log_destroy(this->logger);
	free(this);
}

static ServicioDeResolucionDeDeadlocks * new() {
	ServicioDeResolucionDeDeadlocks * servicio = malloc(sizeof(ServicioDeResolucionDeDeadlocks));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeResolucionDeDeadlocks", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_DEBUG);
	servicio->detectarDeadlock = &detectarDeadlock;
	servicio->destruir = &destruirServicioDeResolucionDeDeadlocks;

	return servicio;
}

const struct ServicioDeResolucionDeDeadlocksClass ServicioDeResolucionDeDeadlocksConstructor = { .new = &new };
