import matplotlib.pyplot as plt
plt.style.use("timesnromanstyle.mplstyle")
plt.rcParams.update({
	"text.usetex": True,
})
import numpy as np

import sys
sys.path.append("..")
import ref.planets as planets


def fig_4():
	data = np.loadtxt("results/m.32.out.txt")
	data, energy = data[:, :-2], data[:, -2:]

	ax = plt.axes()
	ax.plot(np.arange(len(energy))*100, energy[:, 0], label="kinetic energy")
	ax.plot(np.arange(len(energy))*100, energy[:, 1], label="potential energy")
	ax.plot(np.arange(len(energy))*100, energy.sum(axis=1), label="total energy")
	ax.legend()
	plt.xlabel("Time [day]")
	plt.ylabel(r"Energy $\left[\textrm{M}_{\textrm{Earth}}\frac{\textrm{AU}^2}{\textrm{day}^2}\right]$")

	plt.tight_layout()
	plt.savefig("fig_4.eps", format="eps")


if __name__ == "__main__":
	fig_4()
