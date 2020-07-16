#!/usr/bin/env bash

cd ../../gameboy/deploy

./gameboy BROKER CATCH_POKEMON Pikachu 9 3 #19b
sleep 1
./gameboy BROKER CATCH_POKEMON Squirtle 9 3 #20b
sleep 1
./gameboy BROKER CAUGHT_POKEMON 10 OK
sleep 1
./gameboy BROKER CAUGHT_POKEMON 11 FAIL
sleep 1
./gameboy BROKER CATCH_POKEMON Bulbasaur 1 7 #21
sleep 1
./gameboy BROKER CATCH_POKEMON Charmander 1 7 #22
sleep 1
./gameboy BROKER GET_POKEMON Pichu #9
sleep 1
./gameboy BROKER GET_POKEMON Raichu #10