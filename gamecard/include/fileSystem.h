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
#include "reintentoOperacion.h"

int iniciarFileSystem(int argc, char* argv[]);

void formatearTallGrass();
void crearFilesMetadataDirectorio();
void leerMetadataFileSystem();
void formatearTallGrass();
void iniciarSemaforosFS();

void cerrarSemaforosFS();

Pokemon* procesarNew(New* unNew, uint32_t idMensaje);
Caught* procesarCatch(Pokemon* unPokemon, uint32_t idMensaje);
Localized* procesarLocalized(Get* unGet, uint32_t idMensaje);

t_bitarray* g_bitmap;
int g_blockSize;
int g_numberOfBlocks;

pthread_mutex_t m_bitmap;
t_list* g_listaSemaforos;
pthread_mutex_t m_listaSemaforos; // TODO: Si concluyo que no es necesario, sacarlo.

#define MAGIC_NUMBER "TALL_GRASS" //No se está usando, se podría agregar una validación sobre el Metadata del FS para que coincida con esto.
#define SHELL_COMMAND_DELETE "exec rm -r /"

#define PATH_ARCHIVO_METADATA "Metadata/Metadata.bin"
#define PATH_ARCHIVO_BITMAP "Metadata/Bitmap.bin"
#define PATH_FILES "Files"
#define PATH_BLOCKS "Blocks"
#define FILE_METADATA "Metadata.bin"

#endif /* GAMECARD_INCLUDE_FILESYSTEM_H_ */
