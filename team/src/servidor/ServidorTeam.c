//
// Created by Alan Zhao on 07/05/2020.
//

#include "servidor/ServidorTeam.h"

static void iniciar(ServidorTeam *this) {
    log_info(this->logger, "Iniciando servidor...");

    this->socketServidor = crearSocketHaciaBroker();
    listen(this->socketServidor, SOMAXCONN);
    log_info(this->logger, "Listo para abrir conexiones");
}

static void atenderConexiones(ServidorTeam *this) {
    // TODO: convertir el request con delibird/libs.
    //  A futuro se va a utilizar un controlador para routear el pedido, como ocurre en GameBoy.


	pthread_t threadAppeared, threadCaught, threadLocalized, threadGameboy;

	pthread_create(&threadAppeared, NULL, (void*) atenderAppeared, NULL);
	pthread_detach(threadAppeared);

	pthread_create(&threadLocalized, NULL, (void*) atenderLocalized, NULL);
	pthread_detach(threadLocalized);

	pthread_create(&threadCaught, NULL, (void*) atenderCaught, NULL);
	pthread_detach(threadCaught);

	pthread_create(&threadGameboy, NULL, (void*) atenderGameboy, NULL);
	pthread_detach(threadGameboy);
}

static void terminar(ServidorTeam *this) {
    log_info(this->logger, "Terminando servidor...");
    log_destroy(this->logger);
}

static ServidorTeam new() {
    return (ServidorTeam) {
            .logger = log_create(TEAM_INTERNAL_LOG_FILE, "ServidorTeam", SHOW_INTERNAL_CONSOLE, LOG_LEVEL_INFO),
            &iniciar,
            &atenderConexiones,
            &terminar
    };
}

const struct ServidorTeamClass ServidorTeamConstructor = {.new=&new};

//Se deben escuchar los 3 tipos de mensajes
//debe haber un socket que escuche la aparicion de nuevos pokemones
//debe crear un hilo para cada entrenador
//finaliza al cumplir obj global -> obt todos los pokemos requeridos
//planif 1 entrenados -> se mueve al objetivo -> cada mov 1 ciclo de cpu -> retardo de tiempo x


int crearSocketHaciaBroker() {
	struct addrinfo hints;
	struct addrinfo* server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(IP_BROKER, PUERTO_BROKER, &hints, &server_info);
	int socketDeEscucha = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);
	if (connect(socketDeEscucha, server_info->ai_addr, server_info->ai_addrlen)
			== -1)
		puts("error");

	freeaddrinfo(server_info);
	return socketDeEscucha;
}


int iniciarSocketDeEscucha() {
	int socketDeEscucha = crearSocketHaciaBroker();
	//conectarnos al broker y hacer handshake

	send(socketDeEscucha, (int*) 200, sizeof(int), 0);

	printf("Envie: %d", 200); // prueba de envio de un valor para verificar la conexion

	int recibido;
	recv(socketDeEscucha, &recibido, sizeof(int), MSG_WAITALL);

	printf("Recibi: %d", recibido);

//antes del return validar que se recibio bien.
	return socketDeEscucha;

}


void atenderAppeared() {
	int socketDeEscucha = iniciarSocketDeEscucha();
	esperarBrokerAppeared(socketDeEscucha);
}

void esperarBrokerAppeared(int socketDeEscucha) {
	while (1) {
		Pokemon* unPokemon = recv_pokemon(socketDeEscucha,0);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloAppeared, unPokemon);
		pthread_detach(thread);
	}
}

void procesarHiloAppeared(Pokemon* unPokemon) {
	//procesar
	//crear socket descartable al broker
	int socketDescartable = crearSocketHaciaBroker();
	//enviar respuesta al broker
	//esperar confirmacion del broker?
	//liberar memoriar y cerrar socket
	//termina el hilo
}

void atenderCaught() {
	int socketDeEscucha = iniciarSocketDeEscucha();
	esperarBrokerCaught(socketDeEscucha);
}

void esperarBrokerCaught(int socketDeEscucha) {

	while (1) {
		Caught* unCaught = recv_Caught(socketDeEscucha);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloCaught, unCaught);
		pthread_detach(thread);
	}
}

void procesarHiloCaught(Caught* unCaught) {
	//procesar
	//crear socket descartable al broker
	int socketDescartable = crearSocketHaciaBroker();
	//enviar respuesta al broker
	//esperar confirmacion del broker?
	//liberar memoriar y cerrar socket
	//termina el hilo
}

void atenderLocalized() {
	int socketDeEscucha = iniciarSocketDeEscucha();
	esperarBrokerLocalized(socketDeEscucha);
}

void esperarBrokerLocalized(int socketDeEscucha) {

	while (1) {
		Localized* unLocalized = recv_Localized(socketDeEscucha);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) procesarHiloLocalized, unLocalized);
		pthread_detach(thread);
	}
}

void procesarHiloLocalized(Localized* unLocalized) {
	//procesar
	//crear socket descartable al broker
	int socketDescartable = crearSocketHaciaBroker();
	//enviar respuesta al broker
	//esperar confirmacion del broker?
	//liberar memoriar y cerrar socket
	//termina el hilo
}

void atenderGameboy() {
	return;
}

