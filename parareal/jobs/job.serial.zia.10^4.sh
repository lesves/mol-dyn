#!/bin/bash
#PBS -N parareal_serial_test
#PBS -l select=1:ncpus=1:ompthreads=1:mem=1gb:cluster=zia
#PBS -l walltime=00:15:00

DATADIR=/storage/praha1/home/skywalker/mol-dyn/parareal

# config
NSEG=32
MAX_ITERS=16
COARSE_STEPS=10000
FINE_STEPS=100000
TIME=10000
EPS=1e-3
LOG_PERIOD=100

# run
OUT="out.serial.zia.10^4.txt"
FLAGS="--serial"

source $DATADIR/jobs/base_job.sh
