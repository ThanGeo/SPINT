#!/bin/bash

if [ $# -eq 0 ] ; then
    # default execution
    echo mpirun.mpich -np 10 ./program
    mpirun.mpich -np 10 ./program;
elif [ $1 == test1 ] ; then
    echo "Full cluster Intersection Join between T1NA and T2NA using default APRIL (N=16, P=1, uncompressed)"
    mpirun.mpich -np 10 ./program -P -b 10000 -j /home/thanasis/Desktop/PhD/data_files/TIGER/binary/T1NA_fixed_binary.dat /home/thanasis/Desktop/PhD/data_files/TIGER/binary/T2NA_fixed_binary.dat
fi

