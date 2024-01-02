#!/bin/bash

if [ $# -eq 0 ] ; then
    # default execution
    echo mpirun.mpich -np 10 ./program
    mpirun.mpich -np 10 ./program;
fi