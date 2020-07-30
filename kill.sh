#!/usr/bin/env bash

function kill_process() {
    local PROCESS=$1

    case ${PROCESS} in
        broker)
            local PID=$(pidof broker.app);;
        gamecard)
            local PID=$(pidof gameCard.app);;
        team)
            local PID=$(pidof team);;
        team_2)
            local PID=$(pidof team_2);;
        *)
            echo -e "Proceso desconocido: ${RED}${PROCESS}${NC}"
            echo -e "Procesos válidos: ${YELLOW}broker|gamecard|team|team_2${NC}"
            exit 2;;
    esac

    if [ ! -z "${PID}" ]; then
        echo -e "${GREEN}Matando proceso: ${NC}${YELLOW}${PROCESS}${NC}"
        kill -s SIGTERM ${PID}
    else
        echo -e "${RED}Proceso no se encuentra ejecutando: ${NC}${YELLOW}${PROCESS}${NC}"
    fi
}

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

if [ "all" = "$1" ]; then
    kill_process broker
    kill_process gamecard
    kill_process team
    kill_process team_2
else
    kill_process $1
fi