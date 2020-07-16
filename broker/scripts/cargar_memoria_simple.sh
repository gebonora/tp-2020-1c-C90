#!/usr/bin/env bash

cd ../../gameboy/deploy

./gameboy BROKER GET_POKEMON Pikachu

sleep 1

./gameboy BROKER GET_POKEMON Charmander