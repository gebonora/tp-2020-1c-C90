//
// Created by Alan Zhao on 20/07/2020.
//

#include "manejadorDeEventos/registro/RegistradorDeEventos.h"

static void registrarCatchEnEspera(RegistradorDeEventos * this, CapturaPokemon* capturaPokemon) {
    pthread_mutex_lock(&(this->listaCatchEnEspera->mtx));
    list_add(this->listaCatchEnEspera->lista, capturaPokemon);
    pthread_mutex_unlock(&(this->listaCatchEnEspera->mtx));
    char* coor = capturaPokemon->posicion(capturaPokemon);
    log_debug(this->logger, "Se agregó una CapturaPokemon a la listaDeCatchsEnEspera para entrenadorId: %s, pokemon: %s,coordenadas: %s, idCorrelativo: %d.",
              capturaPokemon->idEntrenador, capturaPokemon->pokemonAtrapable->especie, coor, capturaPokemon->idCorrelatividad);
    free(coor);
}

static void registrarGetEnEspera(RegistradorDeEventos * this, char * pokemon, uint32_t idCorrelatividad) {
    MensajeGet * mensajeGet = crearMensajeGet(pokemon, idCorrelatividad);
    pthread_mutex_lock(&(this->listaGetEnEspera->mtx));
    list_add(this->listaGetEnEspera->lista, mensajeGet);
    pthread_mutex_unlock(&(this->listaGetEnEspera->mtx));
    log_debug(this->logger, "Se agregó un MensajeGet a la listaDeGetsEnEspera para pokemon: %s, idCorrelativo: %d.", mensajeGet->nombrePokemon,
              mensajeGet->idCorrelatividad);
}

static void destruir(RegistradorDeEventos * this) {
    log_destroy(this->logger);
    destruirListaSincronizada(this->listaGetEnEspera, destruirMensajeGet);
    destruirListaSincronizada(this->listaCatchEnEspera, (void (*)(void *)) destruirCapturaPokemon);
    free(this);
}

static RegistradorDeEventos * new() {
    RegistradorDeEventos * registradorDeEventos = malloc(sizeof(RegistradorDeEventos));

    registradorDeEventos->logger = log_create(TEAM_INTERNAL_LOG_FILE, "RegistradorDeEventos", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO);
    registradorDeEventos->listaGetEnEspera = iniciarListaSincronizada();
    registradorDeEventos->listaCatchEnEspera = iniciarListaSincronizada();
    registradorDeEventos->registrarCatchEnEspera = &registrarCatchEnEspera;
    registradorDeEventos->registrarGetEnEspera = &registrarGetEnEspera;
    registradorDeEventos->destruir = &destruir;

    return registradorDeEventos;
}

const struct RegistradorDeEventosClass RegistradorDeEventosConstructor = {.new=&new};

ListaSincronizada* iniciarListaSincronizada() {
    ListaSincronizada* listaSincronizada = malloc(sizeof(ListaSincronizada));
    listaSincronizada->lista = list_create();
    pthread_mutex_init(&(listaSincronizada->mtx), NULL);
    return listaSincronizada;
}

void destruirListaSincronizada(ListaSincronizada* listaSincronizada, void (*destructorElemento)(void*)) {
    pthread_mutex_destroy(&(listaSincronizada->mtx));
    list_destroy_and_destroy_elements(listaSincronizada->lista, destructorElemento);
    free(listaSincronizada);
}

void destruirMensajeGet(void* puntero) {
    MensajeGet* mensajeGet = (MensajeGet*) puntero;
    free(mensajeGet->nombrePokemon);
    free(mensajeGet);
}

void destruirCapturaPokemon(CapturaPokemon * capturaPokemon) {
    capturaPokemon->destruir(capturaPokemon);
}

MensajeGet * crearMensajeGet(char * nombrePokemon, uint32_t idCorrelatividad) {
    MensajeGet * mensajeGet = malloc(sizeof(MensajeGet));
    mensajeGet->nombrePokemon = string_duplicate(nombrePokemon);
    mensajeGet->idCorrelatividad = idCorrelatividad;
    return mensajeGet;
}