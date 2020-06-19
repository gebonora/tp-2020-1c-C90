//
// Created by Alan Zhao on 29/04/2020.
//

#ifndef DELIBIRD_SERVICIODECONFIGURACION_H
#define DELIBIRD_SERVICIODECONFIGURACION_H

#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/inotify.h>

#include "utils/hilos/HiloFacil.h"

#define EVENT_SIZE (sizeof (struct inotify_event))
#define BUF_LEN (32 * (EVENT_SIZE + 16))

pthread_mutex_t mutexServicioDeConfiguracion;

typedef struct ServicioDeConfiguracion {
    char * configPath;
    char * logPath;
    t_log *logger;
    t_config *config;
    int fileDescriptor;
    int fileWatch;
    int tiempoDeRefresco;

    int (*obtenerEntero)(struct ServicioDeConfiguracion *this, char *clave);

    double (*obtenerDouble)(struct ServicioDeConfiguracion *this, char *clave);

    char ** (*obtenerArrayDePunteros)(struct ServicioDeConfiguracion *this, char *clave);

    t_list * (*obtenerLista)(struct ServicioDeConfiguracion *this, char *clave);

    char *(*obtenerString)(struct ServicioDeConfiguracion *this, char *clave);

    void (*actualizarConfiguracion)(struct ServicioDeConfiguracion *this);

    void (*monitorearConfiguracion)(struct ServicioDeConfiguracion *this);

    void (*destruir)(struct ServicioDeConfiguracion *this);
} ServicioDeConfiguracion;

extern const struct ServicioDeConfiguracionClass {
    ServicioDeConfiguracion (*new)(char * configPath, char * logPath);
} ServicioDeConfiguracionConstructor;

ServicioDeConfiguracion servicioDeConfiguracion;

#endif //DELIBIRD_SERVICIODECONFIGURACION_H
