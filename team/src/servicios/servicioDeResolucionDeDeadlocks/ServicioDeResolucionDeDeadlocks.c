/*
 * ServicioDeResolucionDeDeadlocks.c
 *
 *  Created on: 16 jul. 2020
 *      Author: GONZALO BONORA
 */

#include "servicios/servicioDeResolucionDeDeadlocks/ServicioDeResolucionDeDeadlocks.h"

t_list* procesarDeadlock(ServicioDeResolucionDeDeadlocks * this, t_list* entrenadoresBloqueados) {
	/* Me llegan los entrenadores llenos y bloqueados desde el planificador.
	 * 		Va a haber uno o mas deadlocks.
	 * 		Quiero llamar al servicio de metricas 1 vez cuando detecto el primer pantallazo de deadlocks, ya que corro el algorimo varias veces
	 * 		pero el numero de deadlocks que hubo a nivel sistema es fijo.
	 *
	 * 		Adentro de esta funcion llamo a resolverDeadlock, que retorna una serie de tareas para resolver los deadlocks.
	 */
	bool esProcesable(void* elem) { // Capaz sacarla afuera.
		Entrenador* entrenador = (Entrenador*) elem;
		return !entrenador->estaIntercambiando;
	}
	ListaDeEntrenadores entrenadoresFiltrados = list_filter(entrenadoresBloqueados, esProcesable); // Esto libera se libera, pero no sus elementos!!!

	ListaDeDependencias listaDeDependencias = this->crearListaDeDependencias(this, entrenadoresFiltrados);

	if (this->primeraVez) {	// Flag aca
		this->detectarEnDetalleYLogear(this, listaDeDependencias);
		this->primeraVez = false;
	}

	// Resolución:
	t_list* tareasADevolver = this->resolverDeadlock(this, listaDeDependencias);

	list_destroy_and_destroy_elements(listaDeDependencias, destruirDependencia);
	list_destroy(entrenadoresFiltrados);
	return NULL;
}

ListaDeDependencias crearListaDeDependencias(ServicioDeResolucionDeDeadlocks* this, ListaDeEntrenadores entrenadoresFiltrados) {
	ListaDeDependencias listaDeDependencias = list_create();
	for (int a = 0; a < list_size(entrenadoresFiltrados); a++) {
		Entrenador* entrenadorActual = (Entrenador*) list_get(entrenadoresFiltrados, a);
		ListaDeStrings listaCapturasEntrenadorActual = obtenerListaDePokemon(entrenadorActual->pokemonesCapturados);
		ListaDeStrings listaObjetivosEntrenadorActual = obtenerListaDePokemon(entrenadorActual->pokemonesObjetivo);
		ListaDeStrings objetivosFiltrados = restarPrimerListaASegunda(listaObjetivosEntrenadorActual, listaCapturasEntrenadorActual); // Devuelve nueva lista con copia de elementos.

		Dependencias* dependenciasEntrenadorActual = malloc(sizeof(Dependencias));
		dependenciasEntrenadorActual->nombreEntrenador = string_duplicate(entrenadorActual->id);
		dependenciasEntrenadorActual->listaDependencias = list_create();

		for (int b = 0; b < list_size(entrenadoresFiltrados); b++) {
			if (a != b) { // Ignorarse a si mismo. De todas formas no debería tener dependecias con sí mismo, porque se filtraron.
				Entrenador* entrenadorIterando = (Entrenador*) list_get(entrenadoresFiltrados, b);
				ListaDeStrings listaCapturasEntrenadorIterando = obtenerListaDePokemon(entrenadorIterando->pokemonesCapturados);
				ListaDeStrings listaObjetivosEntrenadorIterando = obtenerListaDePokemon(entrenadorIterando->pokemonesObjetivo);
				ListaDeStrings capturasFiltradasEntrenadorIterando = restarPrimerListaASegunda(listaCapturasEntrenadorIterando, listaObjetivosEntrenadorIterando);
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
			list_add(listaDeDependencias, dependenciasEntrenadorActual);
		} else {
			destruirDependencia(dependenciasEntrenadorActual);
		}
	}
	return listaDeDependencias;
}

void detectarEnDetalleYLogear(ServicioDeResolucionDeDeadlocks* this, ListaDeDependencias listaDeDependencias) {
	// listaDeDependencias no se toca!!
	ListaDeListaDeString deadlocksTotales = list_create(); // es lista de listas
	log_info(MANDATORY_LOGGER, "Iniciando el algoritmo de detección de deadlocks...");
	for (int a = 0; a < list_size(listaDeDependencias); a++) {
		Dependencias* dependenciaActual = (Dependencias*) list_get(listaDeDependencias, a);
		bool flag = false;
		ListaDeStrings unDeadlock = list_create();
		for (int b = 0; b < list_size(listaDeDependencias); b++) {
			Dependencias* dependenciaIterada = (Dependencias*) list_get(listaDeDependencias, b);
			if (hayDependenciaEnComun(dependenciaActual, dependenciaIterada)) {
				agregarCopiaDeElementosAListaSinRepetir(unDeadlock, dependenciaIterada->listaDependencias);
			}
		}
		list_add(deadlocksTotales, unDeadlock);
	}
	// Deberia tener en deadlocks una entrada con una lista maxima, quiero sacarle los repetidos a esa lista y estaria ok. Con 1 lista por conjunto de procesos en deadlock.
	ListaDeListaDeString deadlocks = eliminarListasRepetidas(deadlocksTotales);
	char* reporte = obtenerReporteDeadlocks(deadlocks);
	log_info(MANDATORY_LOGGER, "Resultado del algoritmo de detección: %s", reporte);
	free(reporte);
	// Registrar en servicioDeMetricas la cantidad de deadlocks.
	for (int a = 0; a < list_size(deadlocks); a++) {
		this->servicioDeMetricas->registrarDeadlockProducido(this->servicioDeMetricas);
		this->servicioDeMetricas->registrarDeadlockResuelto(this->servicioDeMetricas); // TODO: los deadlocks tienen que ser resueltos, sino el servicio de metricas no es llamado.
	}
	list_destroy_and_destroy_elements(deadlocksTotales, destruirListaDeStrings);
	list_destroy(deadlocks);
}

t_list* resolverDeadlock(ServicioDeResolucionDeDeadlocks * this, ListaDeDependencias listaDeDependencias) {
	puts("RESOLUCION:");
	imprimirListaDeDependencias(listaDeDependencias);
	return NULL;

}

// Funciones por ahora estáticas.

char* obtenerReporteDeadlocks(t_list* deadlocks) {
	char* reporte = string_new();
	if (list_is_empty(deadlocks)) {
		string_append(&reporte, "No se encontraron deadlocks");
		return reporte;
	}
	string_append_with_format(&reporte, "Se encontraron '%d' deadlocks.", list_size(deadlocks));
	for (int a = 0; a < list_size(deadlocks); a++) {
		string_append_with_format(&reporte, " Deadlock %d entre: ", a + 1);
		t_list* listaEntrenadores = (t_list*) list_get(deadlocks, a);
		for (int b = 0; b < list_size(listaEntrenadores); b++) {
			if (b == 0) {
				string_append_with_format(&reporte, " '%s'", (char*) list_get(listaEntrenadores, b));
			} else {
				string_append_with_format(&reporte, ", '%s'", (char*) list_get(listaEntrenadores, b));
			}
		}
		string_append(&reporte, ".");
	}
	return reporte;
}

void destruirDependencia(void * aDestruir) {
	Dependencias* dependencia = (Dependencias*) aDestruir;
	free(dependencia->nombreEntrenador);
	list_destroy_and_destroy_elements(dependencia->listaDependencias, free);
	free(dependencia);
}

ListaDeListaDeString eliminarListasRepetidas(ListaDeListaDeString listaDeListas) {
	// Retorna un puntero con un subconjunto de los elementos de la lista que llega por parámetro.
	// Fuera de esta función liberar con: list_destroy a la lista retornada, y list_destroy_and_elements a la lista que llega por parámetro.
	ListaDeListaDeString listaConMismosElementos = list_duplicate(listaDeListas);
	for (int a = 0; a < list_size(listaConMismosElementos); a++) {
		ListaDeStrings listaActual = (ListaDeStrings) list_get(listaConMismosElementos, a);
		for (int b = 0; b < list_size(listaConMismosElementos); b++) {
			if (a != b) {
				ListaDeStrings listaIterada = (ListaDeStrings) list_get(listaConMismosElementos, b);
				if (hayElementoEnComun(listaActual, listaIterada)) {
					list_remove(listaConMismosElementos, a);
				}
			}
		}
	}
	return listaConMismosElementos;
}

void imprimirListaDeListas(ListaDeListaDeString listaDeListas) {
	// Printea una lista de lista con strings.
	puts("IMPRIENDO LISTA DE LISTAS:");
	printf("TAMANIO LISTA DE LISTAS: %d\n", list_size(listaDeListas));
	for (int a = 0; a < list_size(listaDeListas); a++) {
		t_list* listaActual = (t_list*) list_get(listaDeListas, a);
		printf("LISTA NUMERO:: %d, TAMANIO: %d\n", a, list_size(listaActual));
		imprimirListaStrings(listaActual);
	}
}

void imprimirListaDeDependencias(ListaDeDependencias listaDeDependencias) {
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

bool hayDependenciaEnComun(Dependencias* dependencia1, Dependencias* dependencia2) {
	// Evalua la dependencia para deadlocks,
	t_list* lista1 = copiarListaYElementos(dependencia1->listaDependencias);
	list_add(lista1, string_duplicate(dependencia1->nombreEntrenador));
	t_list* lista2 = copiarListaYElementos(dependencia2->listaDependencias);
	list_add(lista2, string_duplicate(dependencia2->nombreEntrenador));
	bool resultado = hayElementoEnComun(lista1, lista2);
	list_destroy_and_destroy_elements(lista1, free);
	list_destroy_and_destroy_elements(lista2, free);
	return resultado;
}

bool hayDependencias(ListaDeStrings listaCapturas, ListaDeStrings listaObjetivos) {
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

ListaDeStrings obtenerListaDePokemon(ContadorPokemones contador) {
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

void imprimirListaStrings(ListaDeStrings lista) { // Por ahora con printf.
	void imprimirElemento(void* elem) {
		char* string = (char*) elem;
		printf("%s\n", string);
	}
	list_iterate(lista, imprimirElemento);
}

// End funciones por ahora estáticas

static void destruirServicioDeResolucionDeDeadlocks(ServicioDeResolucionDeDeadlocks * this) {
	log_debug(this->logger, "Se procede a destruir al servicio de resolución de deadlocks");
	log_destroy(this->logger);
	free(this);
}

static ServicioDeResolucionDeDeadlocks * new(ServicioDeMetricas* servicioDeMetricas) {
	ServicioDeResolucionDeDeadlocks * servicio = malloc(sizeof(ServicioDeResolucionDeDeadlocks));

	servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDeResolucionDeDeadlocks", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_DEBUG);
	servicio->primeraVez = true;
	servicio->servicioDeMetricas = servicioDeMetricas;
	servicio->procesarDeadlock = &procesarDeadlock;
	servicio->crearListaDeDependencias = &crearListaDeDependencias;
	servicio->detectarEnDetalleYLogear = &detectarEnDetalleYLogear;
	servicio->resolverDeadlock = &resolverDeadlock;
	servicio->destruir = &destruirServicioDeResolucionDeDeadlocks;

	return servicio;
}

const struct ServicioDeResolucionDeDeadlocksClass ServicioDeResolucionDeDeadlocksConstructor = { .new = &new };
