//
// Created by Alan Zhao on 30/04/2020.
//

#include "modelo/controladorGameBoy/ControladorGameBoy.h"

ControladorGameBoy obtenerControladorPara(PedidoGameBoy pedidoGameBoy, t_log * logger) {
    switch (pedidoGameBoy.proceso) {
        case TEAM:
            return controladorTeam;
        default:
            log_error(logger, "No se pudo resolver el controlador para el proceso %s",
                    nombreProceso(pedidoGameBoy.proceso));
            exit(EXIT_FAILURE);
    }
}
