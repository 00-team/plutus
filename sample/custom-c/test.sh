#!/bin/sh

for i in {1..10}; do
    ./run
    rm ./test.bin
done
