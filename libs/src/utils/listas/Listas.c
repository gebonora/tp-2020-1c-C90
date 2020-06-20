//
// Created by Alan Zhao on 19/06/2020.
//

#include "utils/listas/Listas.h"

t_list * list_from(void ** array) {
    t_list * lista = list_create();

    void ** comienzo = array;
    while(*comienzo != NULL) {
        list_add(lista, *comienzo);
        comienzo++;
    }

    return lista;
}