#!/usr/bin/env bash

RELATIVE_MODE="-r"
LIBS_PATH=/home/utnso/tp-2020-1c-C90/libs

if [ "${RELATIVE_MODE}" = "$1" ]; then
    LIBS_PATH=`pwd`
fi

DEPLOY_PATH=${LIBS_PATH}/deploy
MANIFEST=install_manifest.txt

echo "install.sh libs path: " ${LIBS_PATH}
echo "install.sh deploy path: " ${DEPLOY_PATH}

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

mkdir -p ${DEPLOY_PATH}
rm -rf ${DEPLOY_PATH}/*
cd ${DEPLOY_PATH} && cmake .. && sudo make install
install_result=$?
cp ${DEPLOY_PATH}/${MANIFEST} ${LIBS_PATH}/${MANIFEST}
rm -rf ${DEPLOY_PATH}

if [ $install_result -eq 0 ]
then
  echo -e "${GREEN}Libreria compartida -Delibird- instalada!${NC}"
else
  echo -e "${RED}Error al instalar libreria compartida${NC}"
fi
