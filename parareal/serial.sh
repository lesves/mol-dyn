#!/bin/bash
#PBS -N parareal_test
#PBS -l select=1:ncpus=1:ompthreads=1:mem=500mb:scratch_local=500mb
#PBS -l walltime=00:15:00

export OMP_NUM_THREADS=$PBS_NUM_PPN

# define a DATADIR variable: directory where the input files are taken from and where the output will be copied to
DATADIR=/storage/praha1/home/skywalker/mol-dyn/parareal

# append a line to a file "jobs_info.txt" containing the ID of the job, the hostname of the node it is run on, and the path to a scratch directory
# this information helps to find a scratch directory in case the job fails, and you need to remove the scratch directory manually 
echo "$PBS_JOBID is running on node `hostname -f` in a scratch directory $SCRATCHDIR" >> $DATADIR/jobs_info.txt

# (old) load openmpi modules
# module add openmpi

# test if the scratch directory is set
# if scratch directory is not set, issue error message and exit
test -n "$SCRATCHDIR" || { echo >&2 "Variable SCRATCHDIR is not set!"; exit 1; }

# copy program
cp $DATADIR/build/parareal $SCRATCHDIR || { echo >&2 "Error while copying input file(s)!"; exit 2; }
cd $SCRATCHDIR
chmod 750 parareal

# run program
./parareal -i $DATADIR/planets.cfg -o out.txt --nseg 30 --coarse-steps 100 --fine-steps 100000 --time 365000 --log-period 100 --serial || { echo >&2 "Calculation ended up erroneously (with a code $?) !!"; exit 3; }

# move the output to user's DATADIR or exit in case of failure
cp out.txt $DATADIR/ || { echo >&2 "Result file(s) copying failed (with a code $?) !!"; exit 4; }

# clean the SCRATCH directory
clean_scratch
