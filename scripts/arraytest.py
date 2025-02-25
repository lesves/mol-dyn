import matplotlib.pyplot as plt

import numpy as np

from pathlib import Path
import subprocess
import os


PARAREAL_PATH = Path("..") / "parareal"
PROGRAM_PATH = PARAREAL_PATH / "build" / "parareal"
INPUT_PATH = PARAREAL_PATH / "planets.cfg"

SEGMENTS = 16
TIME = 365
FINE = 100

COARSE_RANGE = range(10, 101, 10)
EPSILON_POWERS = range(1, 10)
EPSILONS = [10**(-n) for n in EPSILON_POWERS]


def run_experiment():
	mat = np.zeros((len(COARSE_RANGE), len(EPSILONS)), dtype=np.uint16)
	for i, coarse_steps in enumerate(COARSE_RANGE):
		for j, eps in enumerate(EPSILONS):
			result = subprocess.check_output([
				PROGRAM_PATH, 
				"-i", INPUT_PATH, 
				"-o", "tmp.out", 
				"--nseg", str(SEGMENTS),
				"--time", str(TIME),
				"--max-iters", str(SEGMENTS*2),
				"--eps", str(eps),
				"--coarse-steps", str(coarse_steps),
				"--fine-steps", str(FINE)
			])
			iters = len(result.rstrip(b"\n").split(b"\n"))
			print(f"{coarse_steps=} {eps=} {iters=}")

			mat[i, j] = iters

	os.remove("tmp.out")

	return mat

if __name__ == "__main__":
	mat = run_experiment()
	
	fig, ax = plt.subplots()
	ax.matshow(mat.T, cmap="Reds")

	ax.tick_params(axis='x', which='minor', bottom=False, top=False)
	ax.set_xticks(np.arange(len(COARSE_RANGE)))
	ax.set_xticklabels(np.array(list(COARSE_RANGE))/FINE)

	ax.tick_params(axis='y', which='minor', left=False, right=False)
	ax.set_yticks(np.arange(len(EPSILONS)))
	ax.set_yticklabels([f"$10^{{{-i}}}$" for i in EPSILON_POWERS])
	for (x, y), value in np.ndenumerate(mat):
	    ax.text(x, y, str(value), va="center", ha="center")
	ax.set_title("Required iterations for convergence")
	ax.set_xlabel(r"$\frac{N_\mathcal{C}}{N_\mathcal{F}}$ (ratio of the numbers of steps of the coarse and fine solvers)")
	ax.set_ylabel(r"$\varepsilon$ (convergence threshold)")

	plt.tight_layout()
	plt.show()