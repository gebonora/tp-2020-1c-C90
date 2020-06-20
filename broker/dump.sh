#/!usr/bin/env bash

function get_pid() {
    PID=""
    local BROKER_PID=$(pidof broker.app)
    local VALGRIND_PID=$(pidof valgrind.bin)

    if [ ! -z "${BROKER_PID}" ]; then
        PID=${BROKER_PID}
    fi
    if [ ! -z "${VALGRIND_PID}" ]; then
        PID=${VALGRIND_PID}
    fi
}

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo "Looking for broker PID"

get_pid

if [ -z "${PID}" ]
then
    echo -e "${RED}Broker PID not found${NC}"
else
    echo -e "${GREEN}Broker PID found: ${PID}${NC}"
    echo -e "${GREEN}Sending signal SIGUSR1${NC}"
    kill -s SIGUSR1 ${PID}
    echo -e "${GREEN}Signal sent${NC}"
fi 
