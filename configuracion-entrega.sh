#!/usr/bin/env bash

function show_usage(){
    echo -e "Uso: ${YELLOW}$0 IP_BROKER IP_GAMECARD IP_TEAM VM_GAMEBOY${NC}"
    echo -e "El parámetro VM_GAMEBOY recibe el nombre del módulo con el que comparte VM: ${YELLOW}broker|team|gamecard${NC}"
    echo -e "Opcionalmente podemos pasar como primer parámetro ${YELLOW}-r${NC} para que utilice rutas relativas"
}

function replace_file(){
    local SOURCE=$1
    local TARGET=$2

    if [ -f "${SOURCE}" ]; then
        if [ -f "${TARGET}" ]; then
            echo -e "${GREEN}Reemplazando${NC} archivo: ${YELLOW}${TARGET}${NC}, con: ${YELLOW}${SOURCE}${NC}"
            cp ${SOURCE} ${TARGET}
        else
            echo -e "${RED}No existe el archivo destino${NC}: ${TARGET}"
            exit 2
        fi
    else 
        echo -e "${RED}No existe el archivo origen${NC}: ${SOURCE}"
        exit 2
    fi
}

function replace_property(){
    local KEY=$1
    local NEW_VALUE=$2
    local FILE=$3

    if [ -f "${FILE}" ]; then
        if ! grep -R "^[#]*\s*${KEY}=.*" ${FILE} > /dev/null; then
            echo -e "${GREEN}Clave no encontrada, agregando entrada,${NC} archivo: ${YELLOW}${FILE}${NC}, clave: ${YELLOW}${KEY}${NC}, valor: ${YELLOW}${NEW_VALUE}${NC}${NC}"
            echo "${KEY}=${NEW_VALUE}" >> ${FILE}
        else
            echo -e "${GREEN}Clave encontrada, actualizando entrada,${NC} archivo: ${YELLOW}${FILE}${NC}, clave: ${YELLOW}${KEY}${NC}, valor: ${YELLOW}${NEW_VALUE}${NC}${NC}"

            if [[ "$OSTYPE" == "darwin"* ]]; then
                sed -i '' "s/^[#]*\s*${KEY}=.*/${KEY}=${NEW_VALUE}/" ${FILE}
            else
                sed -i "s/^[#]*\s*${KEY}=.*/${KEY}=${NEW_VALUE}/" ${FILE}
            fi
        fi 
    else 
        echo -e "${RED}No existe el archivo:${NC} ${YELLOW}${FILE}${NC}"
    fi
}

function clean_log_file(){
    local FILE=$1

    if [ -f "${FILE}" ]; then
        echo -e "${GREEN}Limpiando archivo de log:${NC} ${YELLOW}${FILE}${NC}"
        > ${FILE}
    else 
        echo -e "${GREEN}Creando archivo de log:${NC} ${YELLOW}${FILE}${NC}"
        touch ${FILE}
    fi
}

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

if [ "$#" -lt 4 ] || [ "$#" -gt 5 ] ; then
    echo -e "${RED}Número de parámetros incorrecto${NC}"
    show_usage
    exit 2
fi

HOME_PATH=/home/utnso

if [ "$#" -eq 5 ]; then
    if [ "-r" != "$1" ]; then
        echo -e "${RED}Parámetros en orden incorrecto${NC}"
        show_usage
        exit 2
    else
        echo "Utilizando rutas relativas"
        DELIBIRD_PATH=`pwd`
        IP_BROKER=$2
        IP_GAMECARD=$3
        IP_TEAM=$4
        VM_GAMEBOY=$5
    fi
else
    DELIBIRD_PATH=${HOME_PATH}/tp-2020-1c-C90
    IP_BROKER=$1
    IP_GAMECARD=$2
    IP_TEAM=$3
    VM_GAMEBOY=$4
fi

IP_GAMEBOY_BROKER=${IP_BROKER}
IP_GAMEBOY_GAMECARD=${IP_GAMECARD}
IP_GAMEBOY_TEAM=${IP_TEAM}

case ${VM_GAMEBOY} in
    broker)
        IP_GAMEBOY_BROKER=127.0.0.1;;
    gamecard)
        IP_GAMEBOY_GAMECARD=127.0.0.1;;
    team)
        IP_GAMEBOY_TEAM=127.0.0.1;;
    *)
        echo -e "${RED}VM_GAMEBOY, módulo desconocido: ${NC}${VM_GAMEBOY}"
        show_usage
        exit 2;;
esac

BROKER_CONFIG=${DELIBIRD_PATH}/broker/config/broker.config
BROKER_ENTREGA=${DELIBIRD_PATH}/broker/config/entrega_broker.config
BROKER_LOG_FILE=${DELIBIRD_PATH}/broker/logs/broker.log
GAMEBOY_CONFIG=${DELIBIRD_PATH}/gameboy/config/deploy/gameboy.config
GAMEBOY_ENTREGA=${DELIBIRD_PATH}/gameboy/config/deploy/entrega_gameboy.config
GAMEBOY_LOG_FILE=${DELIBIRD_PATH}/gameboy/deploy/logs/gameboy.log
GAMECARD_CONFIG=${DELIBIRD_PATH}/gamecard/config/gameCard.config
GAMECARD_ENTREGA=${DELIBIRD_PATH}/gamecard/config/entrega_gamecard.config
GAMECARD_LOG_FILE=${DELIBIRD_PATH}/gamecard/logs/LOG_GAMECARD.log
TEAM_1_CONFIG=${DELIBIRD_PATH}/team/deploy/config/team.config
TEAM_1_ENTREGA=${DELIBIRD_PATH}/team/deploy/config/entrega_team_1.config
TEAM_1_LOG_FILE=${DELIBIRD_PATH}/team/deploy/logs/team_1.log
TEAM_2_CONFIG=${DELIBIRD_PATH}/team/deploy_team_2/config/team.config
TEAM_2_ENTREGA=${DELIBIRD_PATH}/team/deploy_team_2/config/entrega_team_2.config
TEAM_2_LOG_FILE=${DELIBIRD_PATH}/team/deploy_team_2/logs/team_2.log

echo -e "DELIBIRD_PATH: ${YELLOW}${DELIBIRD_PATH}${NC}"
echo -e "BROKER_CONFIG: ${YELLOW}${BROKER_CONFIG}${NC}"
echo -e "BROKER_ENTREGA: ${YELLOW}${BROKER_ENTREGA}${NC}"
echo -e "GAMEBOY_CONFIG: ${YELLOW}${GAMEBOY_CONFIG}${NC}"
echo -e "GAMEBOY_ENTREGA: ${YELLOW}${GAMEBOY_ENTREGA}${NC}"
echo -e "GAMECARD_CONFIG: ${YELLOW}${GAMECARD_CONFIG}${NC}"
echo -e "GAMECARD_ENTREGA: ${YELLOW}${GAMECARD_ENTREGA}${NC}"
echo -e "GAMECARD_LOG_FILE: ${YELLOW}${GAMECARD_LOG_FILE}${NC}"
echo -e "TEAM_1_CONFIG: ${YELLOW}${TEAM_1_CONFIG}${NC}"
echo -e "TEAM_1_ENTREGA: ${YELLOW}${TEAM_1_ENTREGA}${NC}"
echo -e "TEAM_1_LOG_FILE: ${YELLOW}${TEAM_1_LOG_FILE}${NC}"
echo -e "TEAM_2_CONFIG: ${YELLOW}${TEAM_2_CONFIG}${NC}"
echo -e "TEAM_2_ENTREGA: ${YELLOW}${TEAM_2_ENTREGA}${NC}"
echo -e "TEAM_2_LOG_FILE: ${YELLOW}${TEAM_2_LOG_FILE}${NC}"
echo -e "IP_BROKER: ${YELLOW}${IP_BROKER}${NC}"
echo -e "IP_GAMEBOY_BROKER: ${YELLOW}${IP_GAMEBOY_BROKER}${NC}"
echo -e "IP_GAMEBOY_GAMECARD: ${YELLOW}${IP_GAMEBOY_GAMECARD}${NC}"
echo -e "IP_GAMEBOY_TEAM: ${YELLOW}${IP_GAMEBOY_TEAM}${NC}"
echo -e "IP_GAMECARD: ${YELLOW}${IP_GAMECARD}${NC}"
echo -e "IP_TEAM: ${YELLOW}${IP_TEAM}${NC}"
echo -e "VM_GAMEBOY: ${YELLOW}${VM_GAMEBOY}${NC}"

replace_file ${BROKER_ENTREGA} ${BROKER_CONFIG}
replace_file ${GAMEBOY_ENTREGA} ${GAMEBOY_CONFIG}
replace_file ${GAMECARD_ENTREGA} ${GAMECARD_CONFIG}
replace_file ${TEAM_1_ENTREGA} ${TEAM_1_CONFIG}
replace_file ${TEAM_2_ENTREGA} ${TEAM_2_CONFIG}

replace_property "IP_BROKER" ${IP_BROKER} ${BROKER_CONFIG}
replace_property "IP_BROKER" ${IP_BROKER} ${GAMECARD_CONFIG}
replace_property "IP_BROKER" ${IP_BROKER} ${TEAM_1_CONFIG}
replace_property "IP_BROKER" ${IP_BROKER} ${TEAM_2_CONFIG}
replace_property "IP_BROKER" ${IP_GAMEBOY_BROKER} ${GAMEBOY_CONFIG}
replace_property "IP_GAMECARD" ${IP_GAMEBOY_GAMECARD} ${GAMEBOY_CONFIG}
replace_property "IP_TEAM" ${IP_GAMEBOY_TEAM} ${GAMEBOY_CONFIG}

clean_log_file ${BROKER_LOG_FILE}
clean_log_file ${GAMEBOY_LOG_FILE}
clean_log_file ${GAMECARD_LOG_FILE}
clean_log_file ${TEAM_1_LOG_FILE}
clean_log_file ${TEAM_2_LOG_FILE}