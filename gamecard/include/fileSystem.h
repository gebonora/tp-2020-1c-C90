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

void crearDirectoriosBase(char*);

#endif /* GAMECARD_INCLUDE_FILESYSTEM_H_ */
