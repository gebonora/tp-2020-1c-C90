#/!usr/bin/env sh

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
YELLOW='\033[0;33m'
NC='\033[0m'

get_pid

if [ "-k" == "$1" ]; then
    if [ -z "${PID}" ]; then
        echo -e "${RED}Broker is not running${NC}"
    else
        echo -e "${YELLOW}Sending SIGTERM signal to broker with PID: ${PID}${NC}"
    
        kill -s SIGTERM ${PID}

        get_pid

        if [ -z "${PID}" ]; then
            echo -e "${GREEN}Broker terminated succesfully${NC}"
        else
            echo -e "${RED}Something went wrong${NC}"
        fi
    fi
elif [ -z "${PID}" ]; then

    case $1 in
        -v)
            echo -e "${YELLOW}Starting broker in valgrind mode${NC}"
            if [ "-d" == "$1" ]; then
                valgrind --show-leak-kinds=all --leak-check=full --log-file="../logs/broker.log" -v ./../broker.app &
            else
                valgrind --show-leak-kinds=all --leak-check=full --log-file="../logs/broker.log" -v ./../broker.app
            fi
            ;;
        -h)
            echo -e "${YELLOW}Starting broker in helgrind mode${NC}"
        
            if [ "-d" == "$1" ]; then
                valgrind --tool="helgrind" --log-file="../logs/broker.log" -v ./../broker.app &
            else
                valgrind --tool="helgrind" --log-file="../logs/broker.log" -v ./../broker.app
            fi
            ;;
        *)
            echo -e "${YELLOW}Starting broker${NC}"

            if [ "-d" == "$1" ]; then
                ./../broker.app &
            else
                ./../broker.app
            fi
            ;;
    esac

    get_pid

    if [ -z "${PID}" ]; then
        echo -e "${RED}Broker failed to start${NC}"
    else
        echo -e "${GREEN}Broker started with PID: ${PID}${NC}"
    fi
else
    echo -e "${YELLOW}Broker already running with PID: ${PID}${NC}"
fi