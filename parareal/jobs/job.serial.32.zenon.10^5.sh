#!/bin/bash
#PBS -N parareal_1_32_test
#PBS -l select=1:ncpus=1:ompthreads=1:mem=1gb:cluster=zenon
#PBS -l walltime=00:05:00

DATADIR=/storage/praha1/home/skywalker/mol-dyn/parareal

# config
source $DATADIR/jobs/32.10^5.cfg.sh

# run
OUT="out.serial.32.zenon.10^5.txt"
FLAGS="--serial"

source $DATADIR/jobs/base_job.sh
