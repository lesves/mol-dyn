#!/bin/bash
cd build
cmake --build .

NSEG=32
COARSE_STEPS=5000
FINE_STEPS=50000
TIME=10000
EPS=1e-3
MAX_ITERS=8
LOG_PERIOD=10

echo "=== parareal ==="
time ./parareal -i ../planets.cfg -o out_p.txt --nseg $NSEG --coarse-steps $COARSE_STEPS --fine-steps $FINE_STEPS --time $TIME --max-iters $MAX_ITERS --eps $EPS --log-period $LOG_PERIOD
echo "===  serial  ==="
time ./parareal --serial -i ../planets.cfg -o out.txt --nseg $NSEG --coarse-steps $COARSE_STEPS --fine-steps $FINE_STEPS --time $TIME --max-iters $MAX_ITERS --eps $EPS --log-period $LOG_PERIOD

echo "=== distance ==="
python3 ../dist.py out_p.txt out.txt

#python3 vis.py out_p.txt
