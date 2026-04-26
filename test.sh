#!/bin/bash
make
for i in tests/*; do
    echo -------------------- TESTING "$i" --------------------
    ./main "$i" | cut -c 1-2000
done
