#!/bin/bash
#PBS -N parareal_long_test
#PBS -l select=1:ncpus=32:ompthreads=32:mem=1gb:cluster=zia
#PBS -l walltime=01:45:00

DATADIR=/storage/praha1/home/skywalker/mol-dyn/parareal

# config
NSEG=32
MAX_ITERS=12
COARSE_STEPS=1000000
FINE_STEPS=10000000
TIME=365250
EPS=1e-3
LOG_PERIOD=1000

# run
OUT="out.long.zia.txt"
FLAGS=""

source $DATADIR/jobs/base_job.sh
