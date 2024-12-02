#!/bin/bash
#PBS -N parareal_32_32_test
#PBS -l select=1:ncpus=32:ompthreads=32:mem=1gb:cluster=zenon
#PBS -l walltime=00:05:00

# config
source 32.10^5.cfg.sh

# run
OUT="out.32.32.zenon.10^5.txt"
FLAGS=""

source base_job.sh
