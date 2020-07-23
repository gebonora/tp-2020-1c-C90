#/!usr/bin/env bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

BROKER_PATH=/home/utnso/tp-2020-1c-C90/broker
BROKER_CONFIG_FILE=${BROKER_PATH}/config/broker.config
BROKER_BACKUP_FILE=${BROKER_CONFIG_FILE}.bkp
CONFIGS_PATH=${BROKER_PATH}/tests/configs
LOGS_PATH=${BROKER_PATH}/tests/logs
DUMPS_PATH=${BROKER_PATH}/tests/dumps
SCRIPTS_PATH=${BROKER_PATH}/scripts
DUMP_SCRIPT=${SCRIPTS_PATH}/dump.sh
ERROR_MESSAGE=""
ERROR=false

declare -a tests=(
                "buddy_fifo:buddy_basico"
                "buddy_lru:buddy_basico"
                "consolidacion_fifo:consolidacion_basico"
                "consolidacion_lru:consolidacion_basico"
                "compactacion_fifo:compactacion_basico"
                "compactacion_lru:compactacion_basico"
                )


echo "Backing up broker current config file: ${BROKER_CONFIG_FILE}"
#cp ${BROKER_CONFIG_FILE} ${BROKER_BACKUP_FILE}


for test in "${tests[@]}" ; do
    TEST_NAME="${test%%:*}"
    SCRIPT_TO_EXECUTE="${test##*:}"
    TEST_DUMP=${DUMPS_PATH}/${TEST_NAME}_dump.log

    echo "Starting test: ${YELLOW} ${TEST_NAME} ${NC}"

    # copio el archivo de config del test, reemplazando el actual del broker

    echo "Replacing broker config file ${YELLOW} ${BROKER_CONFIG_FILE} ${NC} with test configuration ${YELLOW} ${CONFIGS_PATH}/${TEST_NAME}.config ${NC}"

    #cp -r ${CONFIGS_PATH}/${TEST_NAME}.config ${BROKER_CONFIG_FILE}

    echo "Actual configuration is: "

    cat ${BROKER_CONFIG_FILE}

    # ejecuto la prueba correspondiente en script

    echo "Executing script: ${YELLOW} ${SCRIPTS_PATH}/${SCRIPT_TO_EXECUTE}.sh ${NC}"

    #sh ${SCRIPTS_PATH}/${SCRIPT_TO_EXECUTE}.sh

    # ejecuto dump de la memoria redireccionando el output a otro file

    echo "Executing memory dump: ${YELLOW} ${DUMP_SCRIPT} ${NC}"

    #sh ${DUMP_SCRIPT} > ${TEST_DUMP}

    # elimino la primera linea del archivo de dump copiado

    echo "Deleting dump first line (timestamp)"

    #tail -n +2 "${TEST_DUMP}" > "${TEST_DUMP}.tmp" && mv "${TEST_DUMP}.tmp" "${TEST_DUMP}"

    # comparo el archivo de test contra el archivo de dump copiado

    echo "Comparing dump ${YELLOW} ${TEST_DUMP} ${NC} with test log ${YELLOW} ${LOGS_PATH}/${TEST_NAME}.log ${NC}"

    if [ cmp --silent ${TEST_DUMP} ${LOGS_PATH}/${TEST_NAME}.log ]; then
        echo -e "${GREEN}Test ${TEST_NAME} passes${NC}"
    else
        echo -e "${RED}Test ${TEST_NAME} fails${NC}"
    fi

    echo "Completed test: ${TEST_NAME}"

done

# dejo el archivo de broker.config como estaba antes de los tests

echo "Restoring broker config file from backup"
#mv ${BROKER_BACKUP_FILE} ${BROKER_CONFIG_FILE}

#tail -n +2 "$FILE" > "$FILE.tmp" && mv "$FILE.tmp" "$FILE"

#cmp --silent $old $new || echo "files are different"