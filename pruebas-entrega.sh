#!/usr/bin/env bash

function kill_process() {
    local PROCESS=$1

    case ${PROCESS} in
        broker)
            local PID=$(pidof broker.app);;
        gamecard)
            local PID=$(pidof gameCard.app;;
        team_1)
            local PID=$(pidof pidof team_1);;
        team_2)
            local PID=$(pidof pidof team_2);;
        *)
            echo -e "${RED}PROCESS, proceso desconocido: ${NC}${PROCESS}"
            exit 2;;
    esac

    if [ ! -z "${PID}" ]; then
        echo -e "${RED}Killing process: ${NC}${YELLOW}${PROCESS}${NC}"
        kill -s SIGTERM ${PID}
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

BROKER__PATH=${DELIBIRD_PATH}/broker/scripts
GAMECARD_PATH=${DELIBIRD_PATH}/gamecard
TEAM_1_PATH=${DELIBIRD_PATH}/team/deploy
TEAM_2_PATH=${DELIBIRT_PATH}/team/deploy_team_2

echo -e "DELIBIRD_PATH: ${YELLOW}${DELIBIRD_PATH}${NC}"
echo -e "BROKER_PATH: ${YELLOW}${BROKER_PATH}${NC}"
echo -e "GAMECARD_PATH: ${YELLOW}${GAMECARD_PATH}${NC}"
echo -e "TEAM_1_PATH: ${YELLOW}${TEAM_1_PATH}${NC}"
echo -e "TEAM_2_PATH: ${YELLOW}${TEAM_2_PATH}${NC}"

echo -e "Iniciando proceso: ${YELLOW}broker${NC}\n"

cd ${BROKER_PATH} && ./broker.sh -d

echo -e "Iniciando proceso: ${YELLOW}gamecard${NC}\n"

cd ${GAMECARD_PATH} && ./gameCard.app &

echo -e "Iniciando proceso: ${YELLOW}team 1${NC}\n"

cd ${TEAM_1_PATH} && ./team.app &

echo -e "Iniciando proceso: ${YELLOW}team 2${NC}\n"

cd ${TEAM_2_PATH} && ./team.app &

echo -e "Ejecutando script: ${YELLOW}new_pokemon_antes_team.sh${NC}\n"

echo -e "\n${YELLOW}Esperando confirmación de usuario${NC}\n"

echo -e "Ejecutando script: ${YELLOW}new_pokemon_post_team.sh${NC}\n"

#kill_process "broker"
#kill_process "gamecard"
#kill_process "team_1"
#kill_process "team_2"