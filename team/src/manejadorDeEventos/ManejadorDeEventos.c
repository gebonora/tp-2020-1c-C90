//
// Created by Alan Zhao on 10/06/2020.
//

#include "manejadorDeEventos/ManejadorDeEventos.h"

static void notificarNuevoPokemon(ManejadorDeEventos* this) {
	//esto se dispara cuando llega un APPEARED o LOCALIZED, agrega el pokemon al mapa y demás cosas a TODO: definir
	log_debug(this->logger, "Llego un LOCALIZED");
}

static void notificarPokemonCapturado(ManejadorDeEventos* this) {
	//se dispara cuando llega un CATCH, saca al pokemon del mapa y TODO: mas cosas a definir.
}

static void registrarMensajeEnEsperaEnLista(ManejadorDeEventos* this, char* nombrePokemon, uint32_t idAsignado, ListaSincronizada* lista) {
	//creamos un MensajeEsperado y lo agregamos a la lista
	//el nombrePokemon no se libera, dejamos como responsabilidad que lo libere el que llama a esta funcion de afuera.
	//el MensajeEsperado se libera cuando se saca de la lista

	MensajeEsperado* mensajeEsperado = malloc(sizeof(MensajeEsperado));
	mensajeEsperado->nombrePokemon = string_duplicate(nombrePokemon);
	mensajeEsperado->idMensaje = idAsignado;

	pthread_mutex_lock(&(lista->mtx));
	list_add(lista->lista, mensajeEsperado);
	pthread_mutex_unlock(&(lista->mtx));

}

static void procesarLocalizedRecibido(ManejadorDeEventos* this, Localized* unLocalized) {
	//nos fijamos que sea un localized que hayamos pedido y llamamos a notificarNuevoPokemon para cada coordenada.
}

static void procesarAppearedRecibido(ManejadorDeEventos* this, Pokemon* unPokemon) {
	//pasamanos para llamar a notificarNuevoPokemon.
}

static void procesarCaughtRecibido(ManejadorDeEventos* this, Caught* unCaught) {
	//nos fijamos que sea un caught que hayamos pedido y llamamos a notificarPokemonCapturado.
}

static void destruir(ManejadorDeEventos * this) {
	log_destroy(this->logger);
}

static ManejadorDeEventos new() {
	return (ManejadorDeEventos ) {
		.logger = log_create(TEAM_INTERNAL_LOG_FILE, "ManejadorDeEventos", SHOW_INTERNAL_CONSOLE, INTERNAL_LOG_LEVEL),
		.listaGetEnEspera = iniciarListaSincronizada(), //como hago el init de esta struct??
		.listaCatchEnEspera = iniciarListaSincronizada(),
		&notificarNuevoPokemon,
		&notificarPokemonCapturado,
		&registrarMensajeEnEsperaEnLista,
		&procesarLocalizedRecibido,
		&procesarAppearedRecibido,
		&procesarCaughtRecibido,
		&destruir,
	};
}

const struct ManejadorDeEventosClass ManejadorDeEventosConstructor = { .new = &new };

ListaSincronizada* iniciarListaSincronizada(){
	ListaSincronizada* listaSincronizada = malloc(sizeof(ListaSincronizada));
	listaSincronizada->lista = list_create();
	pthread_mutex_init(&(listaSincronizada->mtx),NULL);
	return listaSincronizada;
}
