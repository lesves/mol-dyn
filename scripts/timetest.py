import matplotlib.pyplot as plt

import numpy as np

from pathlib import Path
import subprocess
import time
import os

DIR = Path(__file__).parent
PARAREAL_PATH = DIR.parent / "parareal"
PROGRAM_PATH = PARAREAL_PATH / "build" / "parareal"
INPUT_PATH = PARAREAL_PATH / "planets.cfg"

SEGMENTS = 16
TIME = 365
FINE = 500
EPS = 1e-3

COARSE_RANGE = range(1, FINE+1, 1)
#WINDOW_SIZE = 5


res = []
times = []
for i, coarse_steps in enumerate(COARSE_RANGE):
	start = time.time()
	result = subprocess.check_output([
		PROGRAM_PATH, 
		"-i", INPUT_PATH, 
		"-o", "tmp.out", 
		"--nseg", str(SEGMENTS),
		"--time", str(TIME),
		"--max-iters", str(SEGMENTS*2),
		"--eps", str(EPS),
		"--coarse-steps", str(coarse_steps),
		"--fine-steps", str(FINE),
	])
	end = time.time()
	elapsed = end-start
	iters = len(result.rstrip(b"\n").split(b"\n"))
	print(f"{coarse_steps=} {iters=} {elapsed=}")

	res.append(iters)
	times.append(elapsed)
os.remove("tmp.out")

data = np.array([
	np.array(list(COARSE_RANGE))/FINE,
	res,
	times
])
np.savetxt(DIR / "results" / "timetest-result.txt", data)


#fig, ax = plt.subplots(2,)

#times_avg = np.convolve(times, np.ones(WINDOW_SIZE), "valid") / WINDOW_SIZE

#ratios = np.array(list(COARSE_RANGE))/FINE
#ax[0].plot(ratios, res)
#ax[0].set_xlabel(r"$\frac{N_\mathcal{C}}{N_\mathcal{F}}$")
#ax[0].set_ylabel(r"Number of iterations")
#ax[1].plot(ratios, times)
##ax[1].plot(ratios[WINDOW_SIZE-1:], times_avg)
#ax[1].set_xlabel(r"$\frac{N_\mathcal{C}}{N_\mathcal{F}}$")
#ax[1].set_ylabel(r"Run time [s]")
##plt.savefig("timetest.eps", format="eps")
#plt.show()
