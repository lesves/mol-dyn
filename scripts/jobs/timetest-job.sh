#!/bin/bash
#PBS -N parareal_16_16_test
#PBS -l select=1:ncpus=16:ompthreads=16:mem=1gb:cluster=zia
#PBS -l walltime=00:23:00

DATADIR=/storage/praha1/home/skywalker/mol-dyn/scripts/

export OMP_NUM_THREADS=$PBS_NUM_PPN

mkdir -p $DATADIR/results/
# append a line to a file "jobs_info.txt" containing the ID of the job, the hostname of the node it is run on, and the path to a scratch directory
# this information helps to find a scratch directory in case the job fails, and you need to remove the scratch directory manually 
echo "$PBS_JOBID is running on node `hostname -f`" >> $DATADIR/results/jobs_info.txt

# run program
module add python
pip3 install matplotlib numpy
python3 $DATADIR/arraytest.py
