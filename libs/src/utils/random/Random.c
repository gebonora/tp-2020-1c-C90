//
// Created by Alan Zhao on 20/06/2020.
//

#include "utils/random/Random.h"

/*
 * La precondicion es que el cliente haya inicializado una semilla con srandom(seed), sino no funca.
 */
char * generateUUID(int length) {
    length = abs(length);
    char * uuid = string_new();
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                     "!$%&/()=-_.,^[]*?";
    while (length-- > 0) {
        size_t index = (double) random() / RAND_MAX * (sizeof charset - 1);
        char * caracter = string_from_format("%c", charset[index]);
        string_append(&uuid, caracter);
        free(caracter);
    }
    return uuid;
}