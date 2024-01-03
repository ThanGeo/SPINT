#!/bin/bash

if [ $# -eq 0 ] ; then
    # default execution
    echo mpirun.mpich -np 10 ./program
    mpirun.mpich -np 10 ./program;
elif [ $1 == test ] ; then
    echo mpirun.mpich -np 10 ./program /home/thanasis/Desktop/PhD/data_files/TIGER/binary/T1NA_fixed_binary.dat /home/thanasis/Desktop/PhD/data_files/TIGER/binary/T2NA_fixed_binary.dat
    mpirun.mpich -np 10 ./program ./program /home/thanasis/Desktop/PhD/data_files/TIGER/binary/T1NA_fixed_binary.dat /home/thanasis/Desktop/PhD/data_files/TIGER/binary/T2NA_fixed_binary.dat
fi

