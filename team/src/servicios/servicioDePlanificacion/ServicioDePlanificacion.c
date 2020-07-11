//
// Created by Alan Zhao on 11/07/2020.
//

#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"

void trabajar(ServicioDePlanificacion * this) {
    while(!this->finDeTrabajo) {
        log_info(this->logger, "Trabajo muy duro siendo un servicio");
    }
    sem_post(&this->semaforoFinDeTrabajo);
}

void destruir(ServicioDePlanificacion * this) {
    this->finDeTrabajo = true;
    sem_wait(&this->semaforoFinDeTrabajo);
    log_debug(this->logger, "Se procede a destruir al servicio de planificacion");
    queue_destroy(this->colaDeTrabajo);
    log_destroy(this->logger);
    free(this);
}

static ServicioDePlanificacion * new() {
    ServicioDePlanificacion * servicio = malloc(sizeof(ServicioDePlanificacion));

    servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDePlanificacion", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    servicio->colaDeTrabajo = queue_create();
    servicio->finDeTrabajo = false;
    sem_init(&servicio->semaforoFinDeTrabajo,0 ,0);
    servicio->trabajar = &trabajar;
    servicio->destruir = &destruir;

    crearHilo((void *(*)(void *)) servicio->trabajar, servicio);

    return servicio;
}

const struct ServicioDePlanificacionClass ServicioDePlanificacionConstructor = {.new=&new};