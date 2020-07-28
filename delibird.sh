#!/usr/bin/env bash

RELATIVE_MODE="-r"
HOME_PATH=/home/utnso
DELIBIRD_PATH=${HOME_PATH}/tp-2020-1c-C90
COMMONS_PATH=${HOME_PATH}/so-commons-library

if [ "${RELATIVE_MODE}" == "$1" ]; then
    DELIBIRD_PATH=`pwd`
fi

echo "delibird.sh project path: " ${DELIBIRD_PATH}

GREEN='\033[0;32m'
NC='\033[0m'

if [ "-i" = "$1" ]; then 
    sudo apt-get -y install cmake
    if [ ! -d ${COMMONS_PATH} ]; then
        cd ${HOME_PATH} && git clone https://github.com/sisoputnfrba/so-commons-library
    fi
    cd ${COMMONS_PATH} && sudo make install
else 
    cd ${DELIBIRD_PATH}/libs && ./install.sh $1
    cd ${DELIBIRD_PATH}/gameboy && ./deploy.sh $1 && rm -f deploy/logs/*
    cd ${DELIBIRD_PATH}/broker && make && rm -f logs/*
    cd ${DELIBIRD_PATH}/gamecard && make && rm -f logs/*
    cd ${DELIBIRD_PATH}/team && ./deploy.sh $1
    cd ${DELIBIRD_PATH}/team && cp -r deploy deploy_team_2
    cd ${DELIBIRD_PATH}/team && mv deploy/team deploy/team_1
    cd ${DELIBIRD_PATH}/team && mv deploy_team_2/team deploy/team_2
    cd ${DELIBIRD_PATH}/team && rm -f deploy/logs/* && rm -f deploy_team_2/logs/*

    echo -e "${GREEN}Todos los modulos compilados!${NC}"
fi
