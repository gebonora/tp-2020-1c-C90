#/!usr/bin/env sh

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

function process() {
    TEST_NAME=$1
    SCRIPT_NAME=$2

    SCRIPT_TO_EXECUTE=${SCRIPTS_PATH}/${SCRIPT_NAME}
    TEST_DUMP=${DUMPS_PATH}/${TEST_NAME}_dump.log
    TEST_LOG=${LOGS_PATH}/${TEST_NAME}.txt
    TEST_CONFIG=${CONFIGS_PATH}/${TEST_NAME}.config

    echo -e "TEST_NAME: ${YELLOW} ${TEST_NAME} ${NC}"
    echo -e "SCRIPT_NAME: ${YELLOW} ${SCRIPT_NAME} ${NC}"
    echo -e "SCRIPT_TO_EXCUTE: ${YELLOW} ${SCRIPT_TO_EXECUTE} ${NC}"
    echo -e "TEST_DUMP: ${YELLOW} ${TEST_DUMP} ${NC}"
    echo -e "TEST_LOG: ${YELLOW} ${TEST_LOG} ${NC}"
    echo -e "TEST_CONFIG: ${YELLOW} ${TEST_CONFIG} ${NC}"

    echo "======================================================================" 

    echo -e "\nSTARTING TEST: ${YELLOW} ${TEST_NAME} ${NC}"

    # copio el archivo de config del test, reemplazando el actual del broker

    echo -e "\nReplacing broker config file ${YELLOW} ${BROKER_CONFIG_FILE} ${NC} with test configuration ${YELLOW} ${TEST_CONFIG} ${NC}"

    cp ${TEST_CONFIG} ${BROKER_CONFIG_FILE}

    # ejecuto proceso broker

    echo -e "\n${YELLOW}Starting up broker process ${NC}\n"

    cd ${BROKER_SCRIPTS} && ./broker.sh -d

    # ejecuto la prueba correspondiente en script

    echo -e "\n\nExecuting script: ${YELLOW} ${SCRIPT_TO_EXECUTE} ${NC} and performing memory dump"

    cd ${SCRIPTS_PATH} && ./${SCRIPT_NAME} && ./dump.sh

    # creo archivo de dump para el test y ejecuto dump de la memoria redireccionando el output a otro file

    echo -e "\nCreating dump file for test: ${YELLOW} ${TEST_LOG} ${NC}"

    > ${TEST_DUMP}

    echo -e "\nCopying dump file ${YELLOW} ${DUMP_FILE} ${NC} to test folder ${YELLOW} ${TEST_DUMP} ${NC}\n"

    cat ${DUMP_FILE}

    cp ${DUMP_FILE} ${TEST_DUMP}

    # elimino la primera linea del archivo de dump copiado

    echo -e "\nDeleting dump first line (timestamp)"

    tail -n +2 "${TEST_DUMP}" > "${TEST_DUMP}.tmp" && mv "${TEST_DUMP}.tmp" "${TEST_DUMP}"

    # comparo el archivo de test contra el archivo de dump copiado

    echo -e "\nComparing dump ${YELLOW} ${TEST_DUMP} ${NC} with test log ${YELLOW} ${TEST_LOG} ${NC}"

    if cmp -s ${TEST_DUMP} ${TEST_LOG}
    then
        echo -e "\nCOMPLETED TEST ${YELLOW} ${TEST_NAME} ${NC} with status: ${GREEN} PASS ${NC}"
        PASSED_TESTS+=" ${TEST_NAME}"
    else
        echo -e "\nCOMPLETED TEST ${YELLOW} ${TEST_NAME} ${NC} with status: ${RED} FAIL ${NC}"
        FAILED_TESTS+=" ${TEST_NAME}"
    fi

    echo -e "\n${YELLOW}Shuting down broker process ${NC}\n"

    cd ${BROKER_SCRIPTS} && ./broker.sh -k
}

HOME_PATH=/home/utnso/tp-2020-1c-C90
BROKER_PATH=${HOME_PATH}/broker
BROKER_SCRIPTS=${BROKER_PATH}/scripts
BROKER_CONFIG_FILE=${BROKER_PATH}/config/broker.config
BROKER_BACKUP_FILE=${BROKER_PATH}/config/backup.config
CONFIGS_PATH=${BROKER_PATH}/config
LOGS_PATH=${BROKER_PATH}/tests/expected
DUMPS_PATH=${BROKER_PATH}/tests/actual
SCRIPTS_PATH=${HOME_PATH}/gameboy/scripts
DUMP_FILE=${BROKER_PATH}/dump.log
FAILED_TESTS=""
PASSED_TESTS=""

declare -a tests=(
                "buddy_fifo:buddy_basico.sh"
                "buddy_lru:buddy_basico.sh"
                "consolidacion_fifo:consolidacion_basico.sh"
                "consolidacion_lru:consolidacion_basico.sh"
                "compactacion_fifo:compactacion_basico.sh"
                "compactacion_lru:compactacion_basico.sh"
                )


echo -e "\nCreating dumps directory if it does not exist"

mkdir -p ${DUMPS_PATH}

echo -e "\nCleaning dump directory"

rm ${DUMPS_PATH}/*

echo -e "\nBacking up broker current config file: ${YELLOW} ${BROKER_CONFIG_FILE} ${NC}"

cp ${BROKER_CONFIG_FILE} ${BROKER_BACKUP_FILE}

for test in "${tests[@]}" ; do
    TEST_NAME="${test%%:*}"
    SCRIPT_NAME="${test##*:}"
    process ${TEST_NAME} ${SCRIPT_NAME}
    sleep 5
done

echo -e "Dump files saved in: ${YELLOW} ${DUMPS_PATH} ${NC}"

# dejo el archivo de broker.config como estaba antes de los tests

echo -e "\nRestoring broker config file from backup"
mv ${BROKER_BACKUP_FILE} ${BROKER_CONFIG_FILE}

echo -e "\n${GREEN}Passed Tests: ${NC} ${YELLOW} ${PASSED_TESTS} ${NC}"
echo -e "${RED}Failed Tests: ${NC} ${YELLOW} ${FAILED_TESTS} ${NC}"