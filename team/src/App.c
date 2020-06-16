#include "app/App.h"

int main() {
    INTERNAL_LOGGER = log_create(TEAM_INTERNAL_LOG_FILE, "Team.app", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    mostrarTitulo(INTERNAL_LOGGER);
    log_info(INTERNAL_LOGGER, "========================= Inicio de ejecución ============================");

    // Config
    log_debug(INTERNAL_LOGGER, "Levantando configuracion...");

    servicioDeConfiguracion = ServicioDeConfiguracionConstructor.new(TEAM_CONFIG_FILE, TEAM_INTERNAL_LOG_FILE);

    pthread_mutex_init(&MTX_INTERNAL_LOG, NULL);

    // Logger obligatorio
    log_debug(INTERNAL_LOGGER, "Configurando logger obligatorio...");
    char * mandatoryLogPath = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, LOG_FILE);
    MANDATORY_LOGGER = log_create(mandatoryLogPath, "LogObligatorio", 1, LOG_LEVEL_INFO);

    // Setup de los componentes del sistema
    log_info(INTERNAL_LOGGER, "Inicializando componentes del sistema...");
    inicializarAlgoritmosDePlanificacion();

    // Reservado a pruebas de integracion
    log_info(INTERNAL_LOGGER, "Realizando pruebas de integracion antes de comenzar...");
    testDeIntegracion();
    log_info(INTERNAL_LOGGER, "Pruebas finalizadas");

    // Estado inicial del proceso Team
    log_info(INTERNAL_LOGGER, "Instanciando entrenadores y armando el equipo...");
    Equipo equipo = crearEquipoPorConfiguracion();

    // Server
    configurarServer();
    atenderConexiones();

    // Por cada pokemon del objetivo global, enviar un GET [POKEMON].

    // Liberacion

    log_debug(INTERNAL_LOGGER, "Finalizando proceso Team...");

    log_debug(INTERNAL_LOGGER, "Liberando logger obligatorio");
    log_destroy(MANDATORY_LOGGER);
    log_debug(INTERNAL_LOGGER, "Liberando servicios");
    eliminarConfigServer();
    servicioDeConfiguracion.destruir(&servicioDeConfiguracion);
    log_debug(INTERNAL_LOGGER, "Saliendo...");
    log_info(INTERNAL_LOGGER, "============================ Fin de ejecución ============================");
    log_destroy(INTERNAL_LOGGER);
    pthread_mutex_destroy(&MTX_INTERNAL_LOG);
    return 0;
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
