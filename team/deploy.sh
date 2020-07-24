#!/usr/bin/env bash

RELATIVE_MODE="-r"
GAMEBOY_PATH=/home/utnso/tp-2020-1c-C90/team

if [ "${RELATIVE_MODE}" = "$1" ]; then
  echo PATO
    GAMEBOY_PATH=`pwd`
fi

DEPLOY_PATH=${GAMEBOY_PATH}/deploy

echo "deploy.sh team path: " ${GAMEBOY_PATH}
echo "deploy.sh deploy path: " ${DEPLOY_PATH}

GREEN='\033[0;32m'
NC='\033[0m'

mkdir -p ${DEPLOY_PATH}
rm -rf ${DEPLOY_PATH}/*
cd ${DEPLOY_PATH} && cmake .. && make
install_result=$?
if [ $install_result -eq 0 ]
then
  echo "${GREEN}Proceso Team deployado!${NC}"
else
  echo "${RED}Error al deployar Team${NC}"
fi
