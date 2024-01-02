#!/bin/bash

if ! [ -d "build" ]; then 
    mkdir build
fi

if [ $# -eq 0 ]; then
    make all;
    mv *.o build/;
    make link;
else
    if [[ $1 == clean ]]; then
        make clean;
    fi
fi