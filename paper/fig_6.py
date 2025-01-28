import matplotlib.pyplot as plt
plt.style.use("timesnromanstyle.mplstyle")

import numpy as np


def fig_6():
	ratios, iters, times = np.loadtxt("results/timetest-result.txt").reshape((3, -1))

	fig, ax = plt.subplots(2,)
	#times_avg = np.convolve(times, np.ones(WINDOW_SIZE), "valid") / WINDOW_SIZE

	ax[0].plot(ratios, iters)
	ax[0].set_xlabel(r"$\frac{N_\mathcal{C}}{N_\mathcal{F}}$")
	ax[0].set_ylabel(r"No. of iterations")
	ax[1].plot(ratios, times)
	#ax[1].plot(ratios[WINDOW_SIZE-1:], times_avg)
	ax[1].set_xlabel(r"$\frac{N_\mathcal{C}}{N_\mathcal{F}}$")
	ax[1].set_ylabel(r"Run time [s]")
	plt.savefig("fig_6.eps", format="eps")


if __name__ == "__main__":
	fig_6()
