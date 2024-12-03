#!/bin/bash
#PBS -N parareal_64_64_test
#PBS -l select=1:ncpus=64:ompthreads=64:mem=1gb:cluster=zia
#PBS -l walltime=00:09:00

DATADIR=/storage/praha1/home/skywalker/mol-dyn/parareal

# config
NSEG=64
MAX_ITERS=16
COARSE_STEPS=5000
FINE_STEPS=50000
TIME=10000
EPS=1e-3
LOG_PERIOD=100

# run
OUT="out.64.64.zia.10^4.txt"
FLAGS=""

source $DATADIR/jobs/base_job.sh
