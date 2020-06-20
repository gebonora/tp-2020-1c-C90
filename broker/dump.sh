#/!usr/bin/env bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo "Looking for broker PID"

PID=$(ps -ef | awk '/broker.app$/ { print $2 }')

if [ -z "${PID}" ]
then
    echo -e "${RED}PID not found${NC}"
else
    echo -e "${GREEN}PID found: ${PID}${NC}"
    echo -e "${GREEN}Sending signal SIGUSR1${NC}"
    kill -s SIGUSR1 ${PID}
    echo -e "${GREEN}Signal sent${NC}"
fi 
