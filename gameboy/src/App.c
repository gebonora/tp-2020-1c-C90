#include "app/App.h"

int main() {
    t_log *logger = log_create(GAMEBOY_INTERNAL_LOG_FILE, "GameBoy.app", 0, LOG_LEVEL_INFO);
    mostrarTitulo(logger);
    log_info(logger, "========================= Inicio de ejecución ============================");

    // Config
    log_debug(logger, "Levantando configuracion...");
    t_config *config = config_create(GAMEBOY_CONFIG_FILE);
    servicioDeConfiguracion = ServicioDeConfiguracionConstructor.new(GAMEBOY_CONFIG_FILE, GAMEBOY_INTERNAL_LOG_FILE);

    // Logger obligatorio
    log_debug(logger, "Configurando logger obligatorio...");
    char * mandatoryLogPath = config_get_string_value(config, MANDATORY_LOG);
    MANDATORY_LOGGER = log_create(mandatoryLogPath, "LogObligatorio", 1, LOG_LEVEL_INFO);

    // Liberacion
    log_debug(logger, "Finalizando proceso GameBoy...");
    log_debug(logger, "Liberando servicios");
    servicioDeConfiguracion.destruir(&servicioDeConfiguracion);
    log_debug(logger, "Saliendo...");
    log_info(logger, "============================ Fin de ejecución ============================");

    log_destroy(logger);
    return 0;
}

void mostrarTitulo(t_log * logger) {
    char *title = "\n"
                  "==============================================================\n"
                  " ██████   █████  ███    ███ ███████ ██████   ██████  ██    ██ \n"
                  "██       ██   ██ ████  ████ ██      ██   ██ ██    ██  ██  ██  \n"
                  "██   ███ ███████ ██ ████ ██ █████   ██████  ██    ██   ████   \n"
                  "██    ██ ██   ██ ██  ██  ██ ██      ██   ██ ██    ██    ██    \n"
                  " ██████  ██   ██ ██      ██ ███████ ██████   ██████     ██    \n"
                  "==============================================================";

    log_info(logger, title);
}