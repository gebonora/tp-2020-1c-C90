#!/usr/bin/env bash

GREEN='\033[0;32m'
NC='\033[0m'

#sudo xargs rm < ./install_manifest.txt
sudo rm -f /usr/lib/libdelibird.so
sudo rm -f -r /usr/include/delibird

echo -e "${GREEN}Libreria compartida -Delibird- desinstalada!${NC}"