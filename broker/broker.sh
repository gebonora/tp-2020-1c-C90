#/!usr/bin/env bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

PID=$(pidof broker.app)

if [ -z "${PID}" ]; then

    case $1 in
        -v)
            echo -e "${YELLOW}Starting broker in valgrind mode${NC}"
            #valgrind --show-leak-kinds=all --leak-check=full --log-file="valgrind.log" -v ./broker.app &
            ;;
        -h)
            echo -e "${YELLOW}Starting broker in helgrind mode${NC}"
            #valgrind --tool="helgrind" --show-leak-kinds=all --leak-check=full --log-file="helgrind.log" -v ./broker.app &
            ;;
        *)
            echo -e "${YELLOW}Starting broker${NC}"
            ./broker.app &
            ;;
    esac

    PID=$(pidof broker.app)

    if [ -z "${PID}" ]; then
        echo -e "${RED}Broker failed to start${NC}"
    else
        echo -e "${GREEN}Broker started with PID: ${PID}${NC}"
    fi

elif [ "-k" == "$1" ]; then

    echo -e "${YELLOW}Sending SIGTERM signal to broker with PID: ${PID}${NC}"
    
    kill -s SIGTERM ${PID}

    PID=$(pidof broker.app)

    if [ -z "${PID}" ]
    then
        echo -e "${GREEN}Broker terminated succesfully${NC}"
    else
        echo -e "${RED}Something went wrong${NC}"
    fi

else
    echo -e "${YELLOW}Broker running with PID: ${PID}${NC}"
fi