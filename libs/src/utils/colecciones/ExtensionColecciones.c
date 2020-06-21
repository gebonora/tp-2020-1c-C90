//
// Created by Alan Zhao on 19/06/2020.
//

#include "utils/colecciones/ExtensionColecciones.h"

t_list * list_from(void ** array) {
    t_list * lista = list_create();

    void ** comienzo = array;
    while(*comienzo != NULL) {
        list_add(lista, *comienzo);
        comienzo++;
    }

    return lista;
}

t_list * keys(t_dictionary * dict) {
    t_list * claves = list_create();

    void obtenerClaves(char * clave, void * valor) {
        list_add(claves, clave);
    }

    dictionary_iterator(dict, obtenerClaves);

    return claves;
}