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
            TrabajoPlanificador * trabajo = queue_pop(this->colaDeTrabajo);
            switch (trabajo->tipo) {
                case CAPTURA_POKEMON:
                    log_debug(this->logger, "Se procede a procesar un trabajo de captura pokemon");
                    break;
                case NOTIFY_CAUGHT_RESULT:
                    log_debug(this->logger, "Se procede a dej");
                    break;
                case DEADLOCK_RESOLUTION:
                    log_debug(this->logger, "Hay trabajo para hacer en la cola de trabajo");
                    break;
                default:
                    log_warning(this->logger, "El trabajo asignado no puede resolverse");
                    break;
            }
        }
    }
    sem_post(&this->semaforoFinDeTrabajo);
}

void asignarEquipoAPlanificar(ServicioDePlanificacion * this, Equipo equipo) {
    log_debug(this->logger, "Convirtiendo a los entrenadores en unidades planificables...");
    t_list * unidadesPlanificables = convertirAUnidadesPlanificables(equipo);
    this->planificador.agregarUnidadesPlanificables(&this->planificador, unidadesPlanificables);
    log_info(this->logger, "Los entrenadores fueron enviados a la cola de NEW");
    list_destroy(unidadesPlanificables);
}

void destruir(ServicioDePlanificacion * this) {
    this->finDeTrabajo = true;
    sem_post(&this->semaforoEjecucionHabilitada);
    sem_wait(&this->semaforoFinDeTrabajo);
    log_debug(this->logger, "Se procede a destruir al servicio de planificacion");
    queue_destroy(this->colaDeTrabajo);
    log_destroy(this->logger);
    this->planificador.destruir(&this->planificador, destruirUnidadPlanificable);
    free(this);
}

static ServicioDePlanificacion * new(servicioDeMetricas) {
    ServicioDePlanificacion * servicio = malloc(sizeof(ServicioDePlanificacion));

    servicio->logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServicioDePlanificacion", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    servicio->colaDeTrabajo = queue_create();
    servicio->finDeTrabajo = false;
    sem_init(&servicio->semaforoFinDeTrabajo,1 ,0);
    sem_init(&servicio->semaforoEjecucionHabilitada,1 ,0);
    servicio->planificador = PlanificadorConstructor.new(servicioDeMetricas);
    servicio->asignarEquipoAPlanificar = &asignarEquipoAPlanificar;
    servicio->trabajar = &trabajar;
    servicio->destruir = &destruir;

    crearHilo((void *(*)(void *)) servicio->trabajar, servicio);

    return servicio;
}

const struct ServicioDePlanificacionClass ServicioDePlanificacionConstructor = {.new=&new};

t_list * convertirAUnidadesPlanificables(Equipo equipo) {
    return list_map(equipo, (void *(*)(void *)) HiloEntrenadorPlanificableConstructor.new);
}

void destruirUnidadPlanificable(UnidadPlanificable * unidadPlanificable){
    unidadPlanificable->destruir(unidadPlanificable);
}