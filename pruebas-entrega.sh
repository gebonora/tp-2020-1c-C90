#!/usr/bin/env bash

function kill_process() {
    local PROCESS=$1

    case ${PROCESS} in
        broker)
            local PID=$(pidof broker.app);;
        gamecard)
            local PID=$(pidof gameCard.app);;
        team)
            local PID=$(pidof pidof team);;
        team_2)
            local PID=$(pidof pidof team_2);;
        *)
            echo -e "${RED}Proceso desconocido: ${NC}${PROCESS}"
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

HOME_PATH=/home/utnso
DELIBIRD_PATH=${HOME_PATH}/tp-2020-1c-C90

if [ "${RELATIVE_MODE}" == "$1" ]; then
    echo "Utilizando rutas relativas"
    DELIBIRD_PATH=`pwd`
fi

BROKER_PATH=${DELIBIRD_PATH}/broker
GAMEBOY_PATH=${DELIBIRD_PATH}/gameboy
GAMECARD_PATH=${DELIBIRD_PATH}/gamecard
TEAM_1_PATH=${DELIBIRD_PATH}/team/deploy
TEAM_2_PATH=${DELIBIRD_PATH}/team/deploy_team_2

echo -e "DELIBIRD_PATH: ${YELLOW}${DELIBIRD_PATH}${NC}"
echo -e "BROKER_PATH: ${YELLOW}${BROKER_PATH}${NC}"
echo -e "GAMEBOY_PATH: ${YELLOW}${GAMEBOY_PATH}${NC}"
echo -e "GAMECARD_PATH: ${YELLOW}${GAMECARD_PATH}${NC}"
echo -e "TEAM_1_PATH: ${YELLOW}${TEAM_1_PATH}${NC}"
echo -e "TEAM_2_PATH: ${YELLOW}${TEAM_2_PATH}${NC}"

echo -e "Iniciando proceso: ${YELLOW}broker${NC}"
cd ${BROKER_PATH} && ./broker.app &

sleep 1

echo -e "Iniciando proceso: ${YELLOW}gamecard${NC}"
cd ${GAMECARD_PATH} && ./gameCard.app &

echo -e "Ejecutando script: ${YELLOW}new_pokemon_antes_team.sh${NC}"
cd ${GAMEBOY_PATH}/scripts && time ./new_pokemon_antes_team.sh

sleep 1

echo -e "Iniciando proceso: ${YELLOW}team 1${NC}"
cd ${TEAM_1_PATH} && ./team &

echo -e "Iniciando proceso: ${YELLOW}team 2${NC}"
cd ${TEAM_2_PATH} && ./team_2 &

sleep 2

echo -e "Ejecutando script: ${YELLOW}new_pokemon_post_team.sh${NC}"

read -n 1 -s -r -p "PRESIONAR CUALQUIER TECLA PARA TERMINAR"

kill_process "broker"
kill_process "gamecard"
kill_process "team"
kill_process "team_2"