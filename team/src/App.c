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

    // Server
    //atenderConexiones();

    // Liberacion
    log_debug(INTERNAL_LOGGER, "Finalizando proceso Team...");
    //terminar();
    log_debug(INTERNAL_LOGGER, "Liberando logger obligatorio");
    log_destroy(MANDATORY_LOGGER);
    log_debug(INTERNAL_LOGGER, "Liberando servicios");
    servicioDeConfiguracion.destruir(&servicioDeConfiguracion);
    log_debug(INTERNAL_LOGGER, "Saliendo...");
    log_info(INTERNAL_LOGGER, "============================ Fin de ejecución ============================");

    log_destroy(INTERNAL_LOGGER);
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
