import matplotlib.pyplot as plt
plt.style.use("timesnromanstyle.mplstyle")

import sys
sys.path.append("..")

from ref.parareal import test_problem


def fig_1():
	test_problem(show=False)
	plt.tight_layout()
	plt.savefig("fig_1.eps", format="eps")


if __name__ == "__main__":
	fig_1()
