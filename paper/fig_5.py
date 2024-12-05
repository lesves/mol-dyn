import matplotlib.pyplot as plt
plt.style.use("timesnromanstyle.mplstyle")

import numpy as np


def fig_5():
	ncpus = [1, 4, 16, 32, 64]
	times = [5*60+1, 2*60+54, 1*60+1, 52, 35]

	xs = np.arange(1, 80)
	ys = (1/xs + 1/20) * times[0]
	plt.plot(xs, ys, "r")

	plt.scatter(ncpus[:1], times[:1], color="C3")
	plt.scatter(ncpus[1:], times[1:], color="C0")

	plt.ylabel("Runtime [s]")
	plt.xlabel("n CPUs")

	plt.tight_layout()
	plt.savefig("fig_5.eps", format="eps")


if __name__ == "__main__":
	fig_5()
