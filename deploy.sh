#!/bin/bash

if [ $# -eq 0 ] ; then
    # default execution
    echo mpirun.mpich -np 10 ./program
    mpirun.mpich -np 10 ./program;
elif [ $# -eq 1 ] ; then
    echo Error: only one argument
elif [ $# -eq 2 ] ; then
    # specified arguments for number of processes
    echo mpirun.mpich $1 $2 ./program
    mpirun.mpich $1 $2 ./program;
elif [ $# -eq 3 ] ; then
    echo Error: only three arguments
elif [ $# -eq 4 ] ; then
    # specified arguments for hostfile
    echo mpirun.mpich $1 $2 $3 $4 ./program
    mpirun.mpich $1 $2 $3 $4 ./program
fi