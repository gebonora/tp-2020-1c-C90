#!/bin/bash

# Array pretending to be a Pythonic dictionary
ARRAY=( "cow:moo"
        "dinosaur:roar"
        "bird:chirp"
        "bash:rock" )

for animal in "${ARRAY[@]}" ; do
    KEY="${animal%%:*}"
    VALUE="${animal##*:}"
    printf "%s likes to %s.\n" "$KEY" "$VALUE"
done

printf "%s is an extinct animal which likes to %s\n" "${ARRAY[1]%%:*}" "${ARRAY[1]##*:}"