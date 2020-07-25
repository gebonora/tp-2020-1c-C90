//
// Created by Alan Zhao on 19/06/2020.
//

#include "utils/colecciones/ExtensionColecciones.h"

t_list * list_from(void ** array) {
	t_list * lista = list_create();

	void ** comienzo = array;
	while (*comienzo != NULL) {
		list_add(lista, *comienzo);
		comienzo++;
	}

	return lista;
}

t_list * dictionary_keys(t_dictionary * dict) {
	t_list * claves = list_create();

	void obtenerClaves(char * clave, void * valor) {
		list_add(claves, clave);
	}

	dictionary_iterator(dict, obtenerClaves);

	return claves;
}

void destruirListaDeStrings(void* aDestruir) {
	ListaDeStrings lista = (t_list*) aDestruir;
	list_destroy_and_destroy_elements(lista, free);
}

bool hayElementoEnComun(ListaDeStrings lista1, ListaDeStrings lista2) {
	for (int a = 0; a < list_size(lista1); a++) {
		char* elem = list_get(lista1, a);
		if (perteneceALista(lista2, elem))
			return true;
	}
	return false;
}

void agregarCopiaDeElementosAListaSinRepetir(ListaDeStrings listaQueRecibe, ListaDeStrings listaQueDa) {
	for (int a = 0; a < list_size(listaQueDa); a++) {
		agregarCopiaSinRepetir(listaQueRecibe, (char*) list_get(listaQueDa, a));
	}
}

ListaDeStrings restarPrimerListaASegunda(ListaDeStrings lista1, ListaDeStrings lista2) {
	// Retorna una nueva lista con nuevos elementos.
	// Liberarla con list_destroy_and_elements
	ListaDeStrings listaA = copiarListaYElementos(lista1);
	ListaDeStrings listaB = copiarListaYElementos(lista2);
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

ListaDeStrings copiarListaYElementos(ListaDeStrings lista) {
	ListaDeStrings retorno = list_create();
	for (int a = 0; a < list_size(lista); a++) {
		list_add(retorno, string_duplicate((char*) list_get(lista, a)));
	}
	return retorno;
}

void agregarCopiaSinRepetir(ListaDeStrings lista, char* elem) {
	if (perteneceALista(lista, elem))
		return;
	else {
		list_add(lista, string_duplicate(elem));
	}
}

bool perteneceALista(ListaDeStrings lista, char* elem) {
	bool esIgual(void* string) {
		return string_equals_ignore_case(elem, (char*) string);
	}
	return list_any_satisfy(lista, esIgual);
}
