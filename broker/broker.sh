#/!usr/bin/env bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

case $1 in
    -v)
        echo -e "${GREEN}Starting broker in valgrind mode${NC}"
        valgrind --show-leak-kinds=all --leak-check=full --log-file="valgrind.log" -v ./broker.app &;;
    -h)
        echo -e "${GREEN}Starting broker in helgrind mode${NC}"
        valgrind --tool="helgrind" --show-leak-kinds=all --leak-check=full --log-file="helgrind.log" -v ./broker.app &;;
    *)
        echo -e "${GREEN}Starting broker${NC}"
        ./broker.app &;;
esac

PID=$(pidof broker.app)

if [ -z "${PID}" ]
then
    echo -e "${RED}Broker failed to start${NC}"
else
    echo -e "${GREEN}Broker started with PID: ${PID}${NC}"
fi