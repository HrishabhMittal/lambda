#!/bin/bash
make
for i in tests/*; do
    echo -------------------- TESTING "$i" --------------------
    ./lambda "$i"
done
