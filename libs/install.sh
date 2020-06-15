#!/usr/bin/env bash

readonly DELIBIRD_PATH=/home/utnso/tp-2020-1c-C90
readonly LIBS_PATH=${DELIBIRD_PATH}/libs
readonly DEPLOY_PATH=${LIBS_PATH}/deploy
readonly MANIFEST=install_manifest.txt

GREEN='\033[0;32m'
NC='\033[0m'

mkdir -p ${DEPLOY_PATH}
rm -rf ${DEPLOY_PATH}/*
cd ${DEPLOY_PATH} && cmake .. && sudo make install
cp ${DEPLOY_PATH}/${MANIFEST} ${LIBS_PATH}/${MANIFEST}
rm -rf ${DEPLOY_PATH}
echo -e "${GREEN}Libreria compartida -Delibird- instalada!${NC}"