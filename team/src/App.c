#include "app/App.h"

int main() {
    // Precalentamiento
    warmUp();
    log_info(INTERNAL_LOGGER, "========================= Inicio de ejecución ============================");

    // Config
    log_debug(INTERNAL_LOGGER, "Levantando configuracion...");
    servicioDeConfiguracion = ServicioDeConfiguracionConstructor.new(TEAM_CONFIG_FILE, TEAM_INTERNAL_LOG_FILE);

    // Logger obligatorio
    log_debug(INTERNAL_LOGGER, "Configurando logger obligatorio...");
    char * mandatoryLogPath = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, LOG_FILE);
    MANDATORY_LOGGER = log_create(mandatoryLogPath, "LogObligatorio", 1, LOG_LEVEL_INFO);

    // Setup de los componentes del sistema
    log_info(INTERNAL_LOGGER, "Inicializando componentes del sistema...");
    inicializarComponentesDelSistema();

    // Reservado a pruebas de integracion. Si logica de negocio rompe, explota el programa aca.
    if (CORRER_TESTS) {
        log_info(INTERNAL_LOGGER, "Realizando pruebas de integracion...");
        testDeIntegracion();
    }

    // Estado inicial del proceso Team
    log_info(INTERNAL_LOGGER, "Configurando el estado inicial del proceso Team...");
    configurarEstadoInicialProcesoTeam();

    // Server
    log_info(INTERNAL_LOGGER, "Levantando el server...");
    configurarServer(); //TODO: Usar como fuente de datos el servicio de config
    atenderConexiones(); //TODO: Hacer apagable

    // Por cada pokemon del objetivo global, enviar un GET [POKEMON].
    log_info(INTERNAL_LOGGER, "Solicitando la ubicacion de los pokemones objetivo para comenzar...");
    //TODO: for loop -> cliente broker -> mover logica a objetivo global

    // Main loop. Traba el hilo main asi no se corta el programa. Se libera si se cumple el objetivo global.

    // Liberacion
    log_info(INTERNAL_LOGGER, "Finalizando proceso Team...");
    liberarRecursos();
    log_info(INTERNAL_LOGGER, "============================ Fin de ejecución ============================");
    log_destroy(INTERNAL_LOGGER);
    return 0;
}

void warmUp() {
    INTERNAL_LOGGER = log_create(TEAM_INTERNAL_LOG_FILE, "Team.app", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL);
    mostrarTitulo(INTERNAL_LOGGER);
    if (SHOW_INTERNAL_CONSOLE) {
        log_warning(INTERNAL_LOGGER, "Logs de uso interno por consola activado. Recordar desactivarlo para la entrega.");
    }
    log_warning(INTERNAL_LOGGER, "Nivel de log interno configurado: %s", log_level_as_string(INTERNAL_LOG_LEVEL));
    if (CORRER_TESTS) {
        log_warning(INTERNAL_LOGGER, "Pruebas de integracion: ACTIVADAS");
    }
}

void mostrarTitulo(t_log * logger) {
    char *title = "\n"
                  "====================================\n"
                  "████████╗███████╗ █████╗ ███╗   ███╗\n"
                  "╚══██╔══╝██╔════╝██╔══██╗████╗ ████║\n"
                  "   ██║   █████╗  ███████║██╔████╔██║\n"
                  "   ██║   ██╔══╝  ██╔══██║██║╚██╔╝██║\n"
                  "   ██║   ███████╗██║  ██║██║ ╚═╝ ██║\n"
                  "   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝\n"
                  "====================================";

    log_info(logger, title);
}

void inicializarComponentesDelSistema() {
    srandom(time(NULL));
    inicializarAlgoritmosDePlanificacion();
    log_debug(INTERNAL_LOGGER, "Creando el mapa de coordenadas...");
    mapaProcesoTeam = MapaConstructor.new();
    pthread_mutex_init(&MTX_INTERNAL_LOG, NULL); //TODO: por ahi conviene moverlo a configurarServer()
}

/**
    * Instanciar a los entrenadores a partir de la config - OK
    * Armar el objetivo global - OK
    * Armar los hilos de entrenador planificables - TODO
    * Enviar a los entrenadores a new - TODO
    * Finalmente, con el cliente broker a mano: Por cada pokemon del objetivo global, enviar un GET \[POKEMON\] - TODO
 */
void configurarEstadoInicialProcesoTeam() {
    log_debug(INTERNAL_LOGGER, "Instanciando entrenadores y armando el equipo...");
    equipo = crearEquipoPorConfiguracion();
    log_debug(INTERNAL_LOGGER, "Calculando el objetivo global...");
    objetivoGlobal = ObjetivoGlobalConstructor.new(equipo);
}

void liberarRecursos() {
    // Server
    log_debug(INTERNAL_LOGGER, "Apagando server...");
    apagarServer(); //Cierra hilos server y elimina config server con sus variables.

    // Logger
    log_debug(INTERNAL_LOGGER, "Liberando logger obligatorio...");
    log_destroy(MANDATORY_LOGGER);
    log_debug(INTERNAL_LOGGER, "Liberando servicios...");

    // Servicios
    servicioDeConfiguracion.destruir(&servicioDeConfiguracion);

    // Componentes
    log_debug(INTERNAL_LOGGER, "Liberando componentes del sistema...");
    pthread_mutex_destroy(&MTX_INTERNAL_LOG);

    // Proceso Team
    log_debug(INTERNAL_LOGGER, "Liberando participantes del proceso Team...");
    destruirEquipo(equipo);
    objetivoGlobal.destruirObjetivoGlobal(&objetivoGlobal);
    mapaProcesoTeam.destruir(&mapaProcesoTeam);
}
