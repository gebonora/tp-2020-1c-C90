//
// Created by Alan Zhao on 20/07/2020.
//

#include "test/TestDeIntegracion.h"

void testClienteBroker() {
    ClienteBrokerV2 * clienteBrokerV2 = ClienteBrokerV2Constructor.new();

    RespuestaBroker respuestaGet = clienteBrokerV2->enviarGet(clienteBrokerV2, "Pitochu");

    assert(respuestaGet.esValida == false);

    RespuestaBroker respuestaCatch = clienteBrokerV2->enviarCatch(clienteBrokerV2, "Pitochu", 1,2);

    assert(respuestaCatch.esValida == false);

    clienteBrokerV2->destruir(clienteBrokerV2);
}