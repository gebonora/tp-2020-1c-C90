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

# TODO: hacer un if para ver si ya esta instalado
#sudo apt-get -y install cmake

# TODO: ver si existe la carpeta, en ese caso no clonarlo
#cd ${HOME_PATH} && git clone https://github.com/sisoputnfrba/so-commons-library
# TODO: ver si ya esta instalado
#cd ${COMMONS_PATH} && sudo make install
cd ${DELIBIRD_PATH}/libs && ./install.sh $1
cd ${DELIBIRD_PATH}/gameboy && ./deploy.sh $1
cd ${DELIBIRD_PATH}/broker && make
cd ${DELIBIRD_PATH}/gamecard && make
cd ${DELIBIRD_PATH}/team &&  ./deploy.sh $1

echo -e "${GREEN}Todos los modulos compilados!${NC}"