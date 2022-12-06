#!/bin/sh

for i in {1..10}; do
    python ./main.py
    rm ./test.bin
done
