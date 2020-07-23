//
// Created by Alan Zhao on 20/07/2020.
//

#ifndef TEAM_REGISTRADORDEEVENTOS_H
#define TEAM_REGISTRADORDEEVENTOS_H

#include "app/Global.h"
#include "modelo/pokemon/CapturaPokemon.h"

typedef struct MensajeGet {
    char* nombrePokemon;
    uint32_t idCorrelatividad;
} MensajeGet;

typedef struct ListaSincronizada {
    t_list* lista;
    pthread_mutex_t mtx;
} ListaSincronizada;

typedef struct RegistradorDeEventos {
    t_log * logger;
    ListaSincronizada * listaGetEnEspera;
    ListaSincronizada * listaCatchEnEspera;
    void (*registrarCatchEnEspera)(struct RegistradorDeEventos * this, CapturaPokemon* capturaPokemon);
    void (*registrarGetEnEspera)(struct RegistradorDeEventos * this, char * pokemon, uint32_t idCorrelatividad);
    void (*destruir)(struct RegistradorDeEventos * this);
} RegistradorDeEventos;

extern const struct RegistradorDeEventosClass {
    RegistradorDeEventos * (*new)();
} RegistradorDeEventosConstructor;

RegistradorDeEventos * registradorDeEventosProcesoTeam;

ListaSincronizada* iniciarListaSincronizada();
void destruirListaSincronizada(ListaSincronizada* listaSincronizada, void (*destructorElemento)(void*));
void destruirMensajeGet(void* puntero);
void destruirCapturaPokemon(CapturaPokemon * capturaPokemon);
MensajeGet * crearMensajeGet(char * nombrePokemon, uint32_t idCorrelatividad);

#endif //TEAM_REGISTRADORDEEVENTOS_H
