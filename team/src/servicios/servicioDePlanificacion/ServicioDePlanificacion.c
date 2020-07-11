//
// Created by Alan Zhao on 11/07/2020.
//

#include "servicios/servicioDePlanificacion/ServicioDePlanificacion.h"

void trabajar(ServicioDePlanificacion * this) {
    while(!this->finDeTrabajo) {

        sem_wait(&this->semaforoEjecucionHabilitada);

        if (this->finDeTrabajo) {
            log_debug(this->logger, "Se interrumpió el ciclo de trabajo por fin de trabajo");
            break;
        }

        if (!queue_is_empty(this->colaDeTrabajo)) {
            log_debug(this->logger, "Hay trabajo para hacer en la cola de trabajo");

        }
    }
    sem_post(&this->semaforoFinDeTrabajo);
}

void destruir(ServicioDePlanificacion * this) {
    this->finDeTrabajo = true;
    sem_post(&this->semaforoEjecucionHabilitada);
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
    sem_init(&servicio->semaforoFinDeTrabajo,1 ,0);
    sem_init(&servicio->semaforoEjecucionHabilitada,1 ,0);
    servicio->trabajar = &trabajar;
    servicio->destruir = &destruir;

    crearHilo((void *(*)(void *)) servicio->trabajar, servicio);

    return servicio;
}

const struct ServicioDePlanificacionClass ServicioDePlanificacionConstructor = {.new=&new};