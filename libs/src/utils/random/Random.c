//
// Created by Alan Zhao on 20/06/2020.
//

#include "utils/random/Random.h"

char * generateUUID() {
    char * uuid = string_new();
    int length = 30;
    srand(time(0));

    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                     "!$%&/()=";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        char * caracter = string_from_format("%c", charset[index]);
        string_append(&uuid, caracter);
        free(caracter);
    }
    return uuid;
}