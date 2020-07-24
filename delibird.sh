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
    cd ${DELIBIRD_PATH}/gameboy && ./deploy.sh $1 && rm -f ${DELIBIRD_PATH}/gameboy/deploy/logs/*
    cd ${DELIBIRD_PATH}/broker && make && rm -f ${DELIBIRD_PATH}/broker/logs/*
    cd ${DELIBIRD_PATH}/gamecard && make && rm -f ${DELIBIRD_PATH}/gamecard/logs/*
    cd ${DELIBIRD_PATH}/team &&  ./deploy.sh && rm -f ${DELIBIRD_PATH}/gameboy/deploy/logs/*

    echo -e "${GREEN}Todos los modulos compilados!${NC}"
fi