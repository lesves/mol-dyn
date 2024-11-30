import matplotlib.pyplot as plt
plt.style.use("timesnromanstyle.mplstyle")
import numpy as np

import sys
sys.path.append("..")
import ref.planets as planets


def fig_3():
	data = np.loadtxt("results/32.out.txt")
	data, energy = data[:, :-2], data[:, -2:]

	data = data.reshape((data.shape[0], data.shape[1]//3, 3))

	lim = 1.2*max(data[0, :, :].min(), data[0, :, :].max())

	plt.rcParams["grid.color"] = "#DDD"
	ax = plt.axes(projection="3d")
	ax.set_xlim3d(-lim, lim)
	ax.set_ylim3d(-lim, lim)
	ax.set_zlim3d(-lim, lim)
	plt.xlabel("x [AU]")
	plt.ylabel("y [AU]")

	for i, name in enumerate(planets.names):
		ax.plot3D(data[:, i, 0], data[:, i, 1], data[:, i, 2], label=name.capitalize())
	ax.legend(bbox_to_anchor=(0.1, 1), loc="upper left", bbox_transform=plt.gcf().transFigure)

	plt.savefig("fig_3.eps", format="eps")


if __name__ == "__main__":
	fig_3()
