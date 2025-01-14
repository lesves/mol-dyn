#!/bin/bash
#PBS -N parareal_32_32_test
#PBS -l select=1:ncpus=32:ompthreads=32:mem=1gb:cluster=zia
#PBS -l walltime=00:12:00

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
OUT="out.32.32.zia.10^4.txt"
FLAGS=""

source $DATADIR/jobs/base_job.sh
