#!/bin/bash

if [ $# -eq 0 ]; then
    make all;
else
    if [ $1 == clean ]; then
        make clean;
    fi
fi