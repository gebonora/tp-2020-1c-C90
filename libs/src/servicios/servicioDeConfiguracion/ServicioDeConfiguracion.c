//
// Created by Alan Zhao on 29/04/2020.
//

#include "servicios/servicioDeConfiguracion/ServicioDeConfiguracion.h"

static void monitorearConfiguracion(ServicioDeConfiguracion *this) {

    if (this->fileDescriptor < 0) {
        log_error(this->logger, "Error al inicializar inotify");
    }

    if (this->fileWatch < 0) {
        log_error(this->logger, "Error al crear inotify watch");
    }

    struct timeval time;
    fd_set rfds;
    int ret;

    time.tv_sec = this->tiempoDeRefresco;
    time.tv_usec = 0;

    FD_ZERO (&rfds);
    FD_SET (this->fileDescriptor, &rfds);

    ret = select(this->fileDescriptor + 1, &rfds, NULL, NULL, &time);

    if (ret < 0) {
        log_error(this->logger, "Hubo un problema con el select");
    } else if (!ret) {
        log_debug(this->logger, "Timeout!");
    } else if (FD_ISSET (this->fileDescriptor, &rfds)) {
        log_debug(this->logger, "Evento disponible");

        char buf[BUF_LEN];
        u_int len, i = 0;

        len = read(this->fileDescriptor, buf, BUF_LEN);

        if (len < 0) {
            log_error(this->logger, "Error al realizar read en el file descriptor");
        }

        while (i < len) {
            struct inotify_event *event;

            event = (struct inotify_event *) &buf[i];

            if (event->mask & IN_MODIFY) {
                if (event->mask & IN_ISDIR) {
                    log_info(this->logger, "%s fue modificado", event->name);
                } else {
                    log_info(this->logger, "El archivo %s fue modificado. Se recarga configuracion.", event->name);
                    this->actualizarConfiguracion(this);
                }
                i += EVENT_SIZE + event->len;
            }
        }
    }
}

static void actualizarConfiguracion(ServicioDeConfiguracion *this) {
    pthread_mutex_lock(&mutexServicioDeConfiguracion);
    config_destroy(this->config);
    this->config = config_create(this->configPath);
    pthread_mutex_unlock(&mutexServicioDeConfiguracion);
    log_info(this->logger, "Se actualizó la configuracion");
}

static int obtenerEntero(ServicioDeConfiguracion *this, char *clave) {
    int valor;
    pthread_mutex_lock(&mutexServicioDeConfiguracion);
    valor = config_get_int_value(this->config, clave);
    pthread_mutex_unlock(&mutexServicioDeConfiguracion);
    log_debug(this->logger, "Se obtuvo un valor entero de la configuracion: %s -> %d", clave, valor);
    return valor;
}

static double obtenerDouble(ServicioDeConfiguracion *this, char *clave) {
    double valor;
    pthread_mutex_lock(&mutexServicioDeConfiguracion);
    valor = config_get_double_value(this->config, clave);
    pthread_mutex_unlock(&mutexServicioDeConfiguracion);
    log_debug(this->logger, "Se obtuvo un valor double de la configuracion: %s -> %f", clave, valor);
    return valor;
}

static char ** obtenerArrayDePunteros(ServicioDeConfiguracion *this, char *clave) {
    char ** valor;
    pthread_mutex_lock(&mutexServicioDeConfiguracion);
    valor = config_get_array_value(this->config, clave);
    char * valorComoString = config_get_string_value(this->config, clave);
    pthread_mutex_unlock(&mutexServicioDeConfiguracion);
    log_debug(this->logger, "Se obtuvo un valor lista de la configuracion: %s -> %s", clave, valorComoString);
    return valor;
}

static t_list * obtenerLista(ServicioDeConfiguracion *this, char *clave) {
    char ** puntero = this->obtenerArrayDePunteros(this, clave);
    t_list * lista = list_create();
    while(*puntero != NULL) {
        list_add(lista, *puntero);
        puntero++;
    }
    return lista;
}

static char *obtenerString(ServicioDeConfiguracion *this, char *clave) {
    char *valor;
    pthread_mutex_lock(&mutexServicioDeConfiguracion);
    valor = config_get_string_value(this->config, clave);
    pthread_mutex_unlock(&mutexServicioDeConfiguracion);
    log_debug(this->logger, "Se obtuvo un string de la configuracion: %s -> %s", clave, valor);
    return valor;
}

static void destruir(ServicioDeConfiguracion *this) {
    inotify_rm_watch(this->fileDescriptor, this->fileWatch);
    close(this->fileDescriptor);
    pthread_mutex_destroy(&mutexServicioDeConfiguracion);
    config_destroy(this->config);
    log_destroy(this->logger);
}

static ServicioDeConfiguracion new(char * configPath, char * logPath) {
    pthread_mutex_init(&mutexServicioDeConfiguracion, NULL);
    int fd = inotify_init();
    return (ServicioDeConfiguracion) {
            .configPath = configPath,
            .logPath = logPath,
            .logger = log_create(logPath, "ServicioDeConfiguracion", 0, LOG_LEVEL_INFO),
            .config = config_create(configPath),
            .fileDescriptor = fd,
            .fileWatch = inotify_add_watch(fd, "./config", IN_MODIFY),
            .tiempoDeRefresco = 1,
            &obtenerEntero,
            &obtenerDouble,
            &obtenerArrayDePunteros,
            &obtenerLista,
            &obtenerString,
            &actualizarConfiguracion,
            &monitorearConfiguracion,
            &destruir
    };
}

const struct ServicioDeConfiguracionClass ServicioDeConfiguracionConstructor = {.new=&new};