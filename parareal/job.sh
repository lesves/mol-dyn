#!/bin/bash
#PBS -N parareal_test
#PBS -l select=1:ncpus=32:ompthreads=32:mem=1gb
#PBS -l walltime=00:10:00

export OMP_NUM_THREADS=$PBS_NUM_PPN

# define a DATADIR variable: directory where the input files are taken from and where the output will be copied to
DATADIR=/storage/praha1/home/skywalker/mol-dyn/parareal

# append a line to a file "jobs_info.txt" containing the ID of the job, the hostname of the node it is run on, and the path to a scratch directory
# this information helps to find a scratch directory in case the job fails, and you need to remove the scratch directory manually 
echo "$PBS_JOBID is running on node `hostname -f`" >> $DATADIR/jobs_info.txt

# run program
NSEG=32
COARSE_STEPS=5000
FINE_STEPS=50000
TIME=10000
EPS=1e-3
MAX_ITERS=16
LOG_PERIOD=10

echo "=== parareal ==="
time $DATADIR/build/parareal -i $DATADIR/planets.cfg -o $DATADIR/out_p.txt --nseg $NSEG --coarse-steps $COARSE_STEPS --fine-steps $FINE_STEPS --time $TIME --max-iters $MAX_ITERS --eps $EPS --log-period $LOG_PERIOD

#echo "===  serial  ==="
#time ./parareal --serial -i $DATADIR/giants.cfg -o out.txt --nseg $NSEG --coarse-steps $COARSE_STEPS --fine-steps $FINE_STEPS --time $TIME --max-iters $MAX_ITERS --eps $EPS
#echo "=== distance ==="
#python3 $DATADIR/dist.py out_p.txt out.txt

# clean the SCRATCH directory
# clean_scratch
