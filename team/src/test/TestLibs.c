//
// Created by Alan Zhao on 15/07/2020.
//

#include "test/TestDeIntegracion.h"

#define uuidsIgualesA(uuidBase) \
bool uuidsIguales(void * uuidDeTesteo) { \
    return string_equals(uuidBase, uuidDeTesteo); \
}

void testLibs() {
    t_log * testLogger = log_create(TEAM_INTERNAL_LOG_FILE, "TestLibs", 1, LOG_LEVEL_INFO);

    log_info(testLogger, "Testeando la generacion random de UUIDs");
    t_list * uuids = list_create();
    int muestras = 100;

    for (int i = 0; i < muestras; i++) {
        char * uuid = (char *) generateUUID(30);
        list_add(uuids, uuid);
    }

    for (int a = 0; a < list_size(uuids); a++) {
        char * uuidActual = list_get(uuids, a);
        uuidsIgualesA(uuidActual);
        int cantUUIDsIguales = list_count_satisfying(uuids, uuidsIguales);
        assert(cantUUIDsIguales == 1);
    }

    list_destroy_and_destroy_elements(uuids, free);
    log_destroy(testLogger);
}