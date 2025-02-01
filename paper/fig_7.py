import matplotlib.pyplot as plt
plt.style.use("timesnromanstyle.mplstyle")
import numpy as np

import sys
sys.path.append("..")
from scripts import arraytest


def fig_7():
	try:
		mat = arraytest.run_experiment()
	except FileNotFoundError:
		print("Building figure 7 requires built parareal at parareal/build/parareal. Build it there using `mkdir parareal/build; cd parareal/build; cmake ../src; cmake --build .`", file=sys.stderr)
	
	fig, ax = plt.subplots()
	ax.matshow(mat.T, cmap="Reds")
	ax.set_xticks(np.arange(len(arraytest.COARSE_RANGE)))
	ax.set_xticklabels(np.array(list(arraytest.COARSE_RANGE))/arraytest.FINE, rotation=45)
	ax.set_yticks(np.arange(len(arraytest.EPSILONS)))
	ax.set_yticklabels([f"$10^{{{-i}}}$" for i in arraytest.EPSILON_POWERS])
	for (x, y), value in np.ndenumerate(mat):
	    ax.text(x, y, str(value), va="center", ha="center")
	ax.set_title("Required iterations for convergence")
	ax.set_xlabel(r"$\frac{N_\mathcal{C}}{N_\mathcal{F}}$")
	ax.set_ylabel(r"$\varepsilon$ (convergence threshold)")
	plt.tick_params(axis="both", which="both", bottom=False, top=False, left=False, right=False, labeltop=False, labelbottom=True)
	plt.minorticks_off()
	plt.tight_layout()
	plt.savefig("fig_7.eps", format="eps")


if __name__ == "__main__":
	fig_7()
