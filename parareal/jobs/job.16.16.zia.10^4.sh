#!/bin/bash
#PBS -N parareal_64_64_test
#PBS -l select=1:ncpus=16:ompthreads=16:mem=1gb:cluster=zia
#PBS -l walltime=00:12:00

DATADIR=/storage/praha1/home/skywalker/mol-dyn/parareal

# config
NSEG=16
MAX_ITERS=8
COARSE_STEPS=20000
FINE_STEPS=200000
TIME=10000
EPS=1e-3
LOG_PERIOD=100

# run
OUT="out.16.16.zia.10^4.txt"
FLAGS=""

source $DATADIR/jobs/base_job.sh
