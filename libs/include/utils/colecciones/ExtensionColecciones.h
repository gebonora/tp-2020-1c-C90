//
// Created by Alan Zhao on 19/06/2020.
//

#ifndef DELIBIRD_EXTENSIONCOLECCIONES_H
#define DELIBIRD_EXTENSIONCOLECCIONES_H

#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>

typedef t_list* ListaDeStrings;

// Listas
t_list * list_from(void ** array);
void destruirListaDeStrings(void* aDestruir);
bool hayElementoEnComun(ListaDeStrings lista1, ListaDeStrings lista2);
void agregarCopiaDeElementosAListaSinRepetir(ListaDeStrings listaQueRecibe, ListaDeStrings listaQueDa);
ListaDeStrings restarPrimerListaASegunda(ListaDeStrings lista1, ListaDeStrings lista2);
ListaDeStrings copiarListaYElementos(ListaDeStrings lista);
void agregarCopiaSinRepetir(ListaDeStrings lista, char* elem);
bool perteneceALista(ListaDeStrings lista, char* elem);

// Diccionarios
t_list * dictionary_keys(t_dictionary * dict);

#endif //DELIBIRD_EXTENSIONCOLECCIONES_H
