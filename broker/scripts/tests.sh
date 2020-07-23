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

declare -a tests=(
                "buddy_fifo:buddy_basico"
                "buddy_lru:buddy_basico"
                "consolidacion_fifo:consolidacion_basico"
                "consolidacion_lru:consolidacion_basico"
                "compactacion_fifo:compactacion_basico"
                "compactacion_lru:compactacion_basico"
                )


echo -e "\nBacking up broker current config file: ${BROKER_CONFIG_FILE}"
cp ${BROKER_CONFIG_FILE} ${BROKER_BACKUP_FILE}


for test in "${tests[@]}" ; do
    TEST_NAME="${test%%:*}"
    SCRIPT_NAME="${test##*:}"
    SCRIPT_TO_EXECUTE=${SCRIPTS_PATH}/${SCRIPT_NAME}.sh
    TEST_DUMP=${DUMPS_PATH}/${TEST_NAME}_dump.log
    TEST_LOG=${LOGS_PATH}/${TEST_NAME}.log
    TEST_CONFIG=${CONFIGS_PATH}/${TEST_NAME}.config

    echo "======================================================================" 

    echo -e "\nStarting test: ${YELLOW} ${TEST_NAME} ${NC}"

    # copio el archivo de config del test, reemplazando el actual del broker

    echo -e "\nReplacing broker config file ${YELLOW} ${BROKER_CONFIG_FILE} ${NC} with test configuration ${YELLOW} ${TEST_CONFIG} ${NC}"

    cp -r ${CONFIGS_PATH}/${TEST_NAME}.config ${BROKER_CONFIG_FILE}

    echo -e "\nActual configuration is: "

    cat ${BROKER_CONFIG_FILE}

    # ejecuto la prueba correspondiente en script

    echo -e "\n\nExecuting script: ${YELLOW} ${SCRIPT_TO_EXECUTE} ${NC}"

    sh ${SCRIPTS_PATH}/${SCRIPT_TO_EXECUTE}.sh

    # ejecuto dump de la memoria redireccionando el output a otro file

    echo -e "\nExecuting memory dump: ${YELLOW} ${DUMP_SCRIPT} ${NC}"

    sh ${DUMP_SCRIPT} > ${TEST_DUMP}

    # elimino la primera linea del archivo de dump copiado

    echo -e "\nDeleting dump first line (timestamp)"

    tail -n +2 "${TEST_DUMP}" > "${TEST_DUMP}.tmp" && mv "${TEST_DUMP}.tmp" "${TEST_DUMP}"

    # comparo el archivo de test contra el archivo de dump copiado

    echo -e "\nComparing dump ${YELLOW} ${TEST_DUMP} ${NC} with test log ${YELLOW} ${TEST_LOG} ${NC}"

    if [ cmp --silent ${TEST_DUMP} ${TEST_LOG} ]; then
        echo -e "Completed test ${YELLOW} ${TEST_NAME} ${NC} with status: ${GREEN} PASS ${NC}"
    else
        echo -e "Completed test ${YELLOW} ${TEST_NAME} ${NC} with status: ${RED} FAIL ${NC}"
    fi

    sleep 2

done

# dejo el archivo de broker.config como estaba antes de los tests

echo -e "\nRestoring broker config file from backup"
mv ${BROKER_BACKUP_FILE} ${BROKER_CONFIG_FILE}

#tail -n +2 "$FILE" > "$FILE.tmp" && mv "$FILE.tmp" "$FILE"

#cmp --silent $old $new || echo -e "files are different"