#!/usr/bin/env bash

RELATIVE_MODE="-r"
GAMEBOY_PATH=/home/utnso/tp-2020-1c-C90/gameboy

if [ "${RELATIVE_MODE}" == "$1" ]; then
    GAMEBOY_PATH=`pwd`
fi

DEPLOY_PATH=${GAMEBOY_PATH}/deploy

echo "deploy.sh gameboy path: " ${GAMEBOY_PATH}
echo "deploy.sh deploy path: " ${DEPLOY_PATH}

GREEN='\033[0;32m'
NC='\033[0m'

mkdir -p ${DEPLOY_PATH}
rm -rf ${DEPLOY_PATH}/*
cd ${DEPLOY_PATH} && cmake .. && make
echo -e "${GREEN}Gameboy compilado!${NC}"