#!/usr/bin/env bash

cd ../../gameboy/deploy

./gameboy BROKER CAUGHT_POKEMON 1 OK
sleep 1
./gameboy BROKER CAUGHT_POKEMON 1 FAIL
sleep 1
./gameboy BROKER CATCH_POKEMON Pikachu 2 3
sleep 1
./gameboy BROKER CATCH_POKEMON Squirtle 5 2
sleep 1
./gameboy BROKER CATCH_POKEMON Onyx 4 5
sleep 1
./gameboy SUSCRIPTOR CAUGHT_POKEMON 10
sleep 1
./gameboy BROKER CATCH_POKEMON Charmander 4 5