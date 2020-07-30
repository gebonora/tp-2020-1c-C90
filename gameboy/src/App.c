#include "app/App.h"

int main(int argc, char **argv) {
	INTERNAL_LOGGER = log_create(GAMEBOY_INTERNAL_LOG_FILE, "GameBoy.app", 1, LOG_LEVEL_DEBUG);
	mostrarTitulo(INTERNAL_LOGGER);
	log_info(INTERNAL_LOGGER, "========================= Inicio de ejecución ============================");

	// Config
	log_debug(INTERNAL_LOGGER, "Levantando configuracion...");
	servicioDeConfiguracion = ServicioDeConfiguracionConstructor.new(GAMEBOY_CONFIG_FILE, GAMEBOY_INTERNAL_LOG_FILE);

	// Logger obligatorio
	log_debug(INTERNAL_LOGGER, "Configurando logger obligatorio...");
	char * mandatoryLogPath = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, MANDATORY_LOG);
	MANDATORY_LOGGER = log_create(mandatoryLogPath, "LogObligatorio", 1, LOG_LEVEL_DEBUG);

	// Controlador
	if (argc > 1) {
		ControladorPedidosGameBoy controladorPedidosGameBoy = ControladorPedidosGameBoyConstructor.new();
		PedidoGameBoy pedidoGameBoy = crearPedidoGameBoy(argc, argv);
		controladorPedidosGameBoy.despacharPedido(&controladorPedidosGameBoy, pedidoGameBoy);
		controladorPedidosGameBoy.destruir(&controladorPedidosGameBoy);
		list_destroy(pedidoGameBoy.argumentos);
	} else {
		log_error(INTERNAL_LOGGER, "No se ingresaron argumentos al proceso.");
	}

	// Liberacion
	log_debug(INTERNAL_LOGGER, "Finalizando proceso GameBoy...");
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
			"==============================================================\n"
			" ██████   █████  ███    ███ ███████ ██████   ██████  ██    ██ \n"
			"██       ██   ██ ████  ████ ██      ██   ██ ██    ██  ██  ██  \n"
			"██   ███ ███████ ██ ████ ██ █████   ██████  ██    ██   ████   \n"
			"██    ██ ██   ██ ██  ██  ██ ██      ██   ██ ██    ██    ██    \n"
			" ██████  ██   ██ ██      ██ ███████ ██████   ██████     ██    \n"
			"==============================================================";

	log_info(logger, title);
}
