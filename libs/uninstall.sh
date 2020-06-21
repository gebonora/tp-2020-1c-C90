#!/usr/bin/env bash

GREEN='\033[0;32m'
NC='\033[0m'

sudo xargs rm -f < ./install_manifest.txt

echo -e "${GREEN}Libreria compartida -Delibird- desinstalada!${NC}"