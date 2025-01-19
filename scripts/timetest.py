import matplotlib.pyplot as plt

import numpy as np

from pathlib import Path
import subprocess
import time
import os
import sys
import gc


DIR = Path(__file__).parent
PARAREAL_PATH = DIR.parent / "parareal"
PROGRAM_PATH = PARAREAL_PATH / "build" / "parareal"
INPUT_PATH = PARAREAL_PATH / "planets.cfg"

SEGMENTS = 16
TIME = 365*2
FINE = 800
EPS = 1e-3

COARSE_RANGE = range(0, FINE+1, 8)

N_TRIALS = 10


def run_experiment():
	res = []
	times = []
	for i, coarse_steps in enumerate(COARSE_RANGE):
		gc.disable()
		start = time.perf_counter()
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
		end = time.perf_counter()
		gc.enable()
		elapsed = end-start
		iters = len(result.rstrip(b"\n").split(b"\n"))
		print(f"{coarse_steps=} {iters=} {elapsed=}")

		res.append(iters)
		times.append(elapsed)
	os.remove("tmp.out")

	return np.array([
		np.array(list(COARSE_RANGE))/FINE,
		res,
		times
	])


if __name__ == "__main__":
	if len(sys.argv) <= 1:
		print("not enough arguments", file=sys.stderr)
		sys.exit(1)

	if True:
		results = np.array([run_experiment() for i in range(N_TRIALS)])
		results = results.mean(axis=0)
		np.savetxt(sys.argv[1], results)
	else:
		results = np.loadtxt(sys.argv[1]).reshape((3, -1))

		ratios, iters, times = results

		fig, ax = plt.subplots(2,)

		#times_avg = np.convolve(times, np.ones(WINDOW_SIZE), "valid") / WINDOW_SIZE

		ax[0].plot(ratios, iters)
		ax[0].set_xlabel(r"$\frac{N_\mathcal{C}}{N_\mathcal{F}}$")
		ax[0].set_ylabel(r"Number of iterations")
		ax[1].plot(ratios, times)
		#ax[1].plot(ratios[WINDOW_SIZE-1:], times_avg)
		ax[1].set_xlabel(r"$\frac{N_\mathcal{C}}{N_\mathcal{F}}$")
		ax[1].set_ylabel(r"Run time [s]")
		#plt.savefig("timetest.eps", format="eps")
		plt.show()
