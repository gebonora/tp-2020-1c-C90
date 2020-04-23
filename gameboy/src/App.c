#include "app/App.h"

int main() {
    t_log *logger = log_create("gameboy/logs/gameboy.log", "GameBoy.app", 1, LOG_LEVEL_INFO);
    mostrarTitulo(logger);
    log_info(logger, "========================= Inicio de ejecución ============================");

    // Config - TODO: Migrar a servicio
    log_debug(logger, "Levantando configuracion...");
    t_config *config = config_create(GAMEBOY_CONFIG_FILE);
    char *clave = "PUERTO_TEAM";
    char *valor = config_get_string_value(config, clave);
    log_info(logger, "Se obtuvo un string de la configuracion: %s -> %s", clave, valor);

    //Liberacion
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
