//
// Created by Alan Zhao on 19/06/2020.
//

#ifndef DELIBIRD_EXTENSIONCOLECCIONES_H
#define DELIBIRD_EXTENSIONCOLECCIONES_H

#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

// Listas
t_list * list_from(void ** array);

// Diccionarios
t_list * dictionary_keys(t_dictionary * dict);

#endif //DELIBIRD_EXTENSIONCOLECCIONES_H
