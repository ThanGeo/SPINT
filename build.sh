#!/bin/bash

# check build dir
if ! [ -d "build" ]; then 
    mkdir build
fi

if [ $# -eq 0 ]; then
    # default
    make all;
    mv *.o build/;
    make link;
else
    if [[ $1 == clean ]]; then
        # clean everything
        make clean;
    fi

    if [[ $1 == debug ]]; then
        # debug (for gdb)
        make debug;
        mv *.o build/;
        make link;
    fi
fi