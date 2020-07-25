/*
 * main.c
 *
 *  Created on: 13 abr. 2020
 *      Author: GONZALO BONORA
 */
#include "main.h"

int main(int argc, char* argv[]) {
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
	puts("\n");
	cargarConfig();

	if (iniciarFileSystem(argc, argv) < 0) {
		cerrarLoggers();
		cerrarVariablesConfig();
		exit(-1);
	}

	//testing

	//correrTests();

	//end testing

	atenderConexiones();

	cerrarLoggers();
	cerrarVariablesConfig();
	cerrarBitmap();
	cerrarSemaforosFS();
	return 0;
}

