/*
 * main.c
 *
 *  Created on: 13 abr. 2020
 *      Author: GONZALO BONORA
 */
#include "main.h"

int main111() {
	char *title = "\n"
			"===================================================================\n"
			" ██████   █████  ███    ███ ███████ ███████  █████  ██████  ██████ \n"
			"██       ██   ██ ████  ████ ██      ██      ██   ██ ██  ██  ██   ██\n"
			"██   ███ ███████ ██ ████ ██ █████   ██      ███████ ██████  ██    █\n"
			"██    ██ ██   ██ ██  ██  ██ ██      ██      ██   ██ ██   ██ ██   ██\n"
			" ██████  ██   ██ ██      ██ ███████ ███████ ██   ██ ██    █ ██████ \n"
			"====================================================================";

	iniciarLoggers();
	log_info(loggerMain, title);
	log_info(loggerMain, "Se inició el programa.");
	cargarConfig();
	atenderConexiones();
	cerrarLoggers();
	return 0;
}

