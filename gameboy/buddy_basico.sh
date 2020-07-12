#!/usr/bin/env bash

cd deploy

./gameboy BROKER CAUGHT_POKEMON 1 OK

sleep 1

./gameboy BROKER CAUGHT_POKEMON 1 FAIL

sleep 1

./gameboy BROKER NEW_POKEMON Pikachu 2 3 1

sleep 1

./gameboy BROKER CATCH_POKEMON Onyx 4 5

sleep 1

#./gameboy SUSCRIPTOR NEW_POKEMON 10

./gameboy BROKER CATCH_POKEMON Charmander 4 5