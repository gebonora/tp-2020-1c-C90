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
	}

	//testing

	//testCreacionCoordenadasMultiplesPokemon("pikachu", 40);

	//testCreacionCoordenadaPokemon("charmander", 1, 1, 2);

	//testRemocionCoordenadaPokemon("charmander", 1, 1, 2);
/*
	testRemocionCoordenadaPokemon("pikachu", 1, 0, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 1, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 2, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 3, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 4, 2);
	testRemocionCoordenadaPokemon("pikachu", 1, 5, 2);
*/
	//testGetPokemon("pikachu");

	//end testing

	atenderConexiones();

	cerrarLoggers();
	cerrarVariablesConfig();
	cerrarBitmap();
	return 0;
}

