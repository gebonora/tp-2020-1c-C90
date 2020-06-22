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
#include <fts.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>
#include <pthread.h>

#include <commons/bitarray.h>

#include <delibird/interface.h>
#include "config.h"
#include "logger.h"
#include "server.h"
#include "manejoDeArchivos.h"
#include "bitmap.h"
#include "bloques.h"
#include "mapeo.h"

int iniciarFileSystem(int argc, char* argv[]);

void formatearTallGrass();
void crearFilesMetadataDirectorio();
void leerMetadataFileSystem();
void formatearTallGrass();
void iniciarSemaforosFS();

Pokemon* procesarNew(New* unNew);
Caught* procesarCatch(Pokemon* unPokemon);
Localized* procesarLocalized(Get* unGet);

t_bitarray* g_bitmap;
int g_blockSize;
int g_numberOfBlocks;

pthread_mutex_t m_bitmap;
pthread_mutex_t m_abrirArchivo;


#define MAGIC_NUMBER "TALL_GRASS"
#define SHELL_COMMAND_DELETE "exec rm -r /"

#define PATH_ARCHIVO_METADATA "Metadata/Metadata.bin"
#define PATH_ARCHIVO_BITMAP "Metadata/Bitmap.bin"

#endif /* GAMECARD_INCLUDE_FILESYSTEM_H_ */
