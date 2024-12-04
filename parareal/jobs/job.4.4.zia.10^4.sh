#!/bin/bash
#PBS -N parareal_4_4_test
#PBS -l select=1:ncpus=4:ompthreads=4:mem=1gb:cluster=zia
#PBS -l walltime=00:10:00

DATADIR=/storage/praha1/home/skywalker/mol-dyn/parareal

# config
NSEG=4
MAX_ITERS=1
COARSE_STEPS=20000
FINE_STEPS=200000
TIME=10000
EPS=1e-3
LOG_PERIOD=100

# run
OUT="out.4.4.zia.10^4.txt"
FLAGS=""

source $DATADIR/jobs/base_job.sh
