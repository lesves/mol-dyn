export OMP_NUM_THREADS=$PBS_NUM_PPN

mkdir -p $DATADIR/results/
# append a line to a file "jobs_info.txt" containing the ID of the job, the hostname of the node it is run on, and the path to a scratch directory
# this information helps to find a scratch directory in case the job fails, and you need to remove the scratch directory manually 
echo "$PBS_JOBID is running on node `hostname -f`" >> $DATADIR/results/jobs_info.txt

# run program
time $DATADIR/build/parareal -i $DATADIR/planets.cfg -o $DATADIR/results/$OUT --nseg $NSEG --coarse-steps $COARSE_STEPS --fine-steps $FINE_STEPS --time $TIME --max-iters $MAX_ITERS --eps $EPS --log-period $LOG_PERIOD $FLAGS
