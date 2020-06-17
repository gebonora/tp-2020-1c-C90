/*
 * fileSystem.h
 *
 *  Created on: 19 abr. 2020
 *      Author: GONZALO BONORA
 */

#ifndef GAMECARD_INCLUDE_FILESYSTEM_H_
#define GAMECARD_INCLUDE_FILESYSTEM_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "config.h"
#include "logger.h"
#include <stdbool.h>
#include <delibird/interface.h>
#include "server.h"

void crearDirectoriosBase(char* subPath);
void crearArchivosBase(char* subPath);
int fileExists(char * path);

Pokemon* procesarNew(New* unNew);
Caught* procesarCatch(Pokemon* unPokemon);
Localized* procesarLocalized(Get* unGet);

#endif /* GAMECARD_INCLUDE_FILESYSTEM_H_ */
