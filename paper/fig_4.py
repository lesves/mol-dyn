import matplotlib.pyplot as plt
plt.style.use("timesnromanstyle.mplstyle")
#plt.rcParams.update({
#	"text.usetex": True,
#})
import numpy as np

import sys
sys.path.append("..")
import ref.planets as planets


def fig_4():
	data = np.loadtxt("results/out.32.32.zia.10^4.txt")
	data, energy = data[:, :-2], data[:, -2:]

	log_interval = 100
	dt = 1e4/32e5
	year = 365.242199

	ax = plt.axes()
	ax.plot(np.arange(len(energy))*log_interval * dt / year, energy[:, 0], label="kinetic energy")
	ax.plot(np.arange(len(energy))*log_interval * dt / year, energy[:, 1], label="potential energy")
	ax.plot(np.arange(len(energy))*log_interval * dt / year, energy.sum(axis=1), label="total energy")
	ax.legend()
	plt.xlabel("Time [year]")
	plt.ylabel(r"Energy $\left[\mathrm{M}_{\mathrm{Earth}}\frac{\mathrm{AU}^2}{\mathrm{day}^2}\right]$")

	plt.tight_layout()
	plt.savefig("fig_4.eps", format="eps")


if __name__ == "__main__":
	fig_4()
