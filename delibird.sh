#!/usr/bin/env bash

RELATIVE_MODE="-r"
DELIBIRD_PATH=/home/utnso/tp-2020-1c-C90

if [ "${RELATIVE_MODE}" == "$1" ]; then
    DELIBIRD_PATH=`pwd`
fi

echo "delibird.sh project path: " ${DELIBIRD_PATH}

GREEN='\033[0;32m'
NC='\033[0m'

cd ${DELIBIRD_PATH}/libs && ./install.sh
cd ${DELIBIRD_PATH}/gameboy && ./deploy.sh
cd ${DELIBIRD_PATH}/broker && make
#cd ${DELIBIRD_PATH}/gamecard && make
#cd ${DELIBIRD_PATH}/team && make

echo -e "${GREEN}Todos los modulos compilados!${NC}"