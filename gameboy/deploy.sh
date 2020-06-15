#!/usr/bin/env bash

readonly DELIBIRD_PATH=/home/utnso/tp-2020-1c-C90
readonly DEPLOY_PATH=${DELIBIRD_PATH}/gameboy/deploy

GREEN='\033[0;32m'
NC='\033[0m'

mkdir -p ${DEPLOY_PATH}
rm -rf ${DEPLOY_PATH}/*
cd ${DEPLOY_PATH} && cmake .. && make
echo -e "${GREEN}Gameboy compilado!${NC}"