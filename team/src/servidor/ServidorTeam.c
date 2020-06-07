//
// Created by Alan Zhao on 07/05/2020.
//

#include "servidor/ServidorTeam.h"

static void iniciar(ServidorTeam *this) {
    log_info(this->logger, "Iniciando servidor...");

    int socket_servidor = 0;
    struct addrinfo hints, *serverInfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char *ip = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, IP_BROKER);
    char *puerto = servicioDeConfiguracion.obtenerString(&servicioDeConfiguracion, PUERTO_BROKER);
    getaddrinfo(ip, puerto, &hints, &serverInfo);

    for (p = serverInfo; p != NULL; p = p->ai_next) {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;
        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

    listen(socket_servidor, SOMAXCONN);
    freeaddrinfo(serverInfo);
    this->socketServidor = socket_servidor;
    log_info(this->logger, "Listo para abrir conexiones");
}

static void atenderConexiones(ServidorTeam *this) {
    // TODO: convertir el request con delibird/libs.
    //  A futuro se va a utilizar un controlador para routear el pedido, como ocurre en GameBoy.
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