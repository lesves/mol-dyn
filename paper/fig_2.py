import matplotlib.pyplot as plt
plt.style.use("timesnromanstyle.mplstyle")
import numpy as np

import sys
sys.path.append("..")

from ref.parareal import parareal, verlet, rk4
from ref.simulation import simulation
import ref.planets as planets


def fig_2():
	fig = plt.figure(figsize=plt.figaspect(0.5))

	it = 0

	def plot_one(states):
		nonlocal it
		ax = fig.add_subplot(2, 3, it+1, projection='3d')
		ax.get_xaxis().set_ticks([])
		ax.get_yaxis().set_ticks([])
		ax.get_zaxis().set_ticks([])
		ax.title.set_text(f"iter {it}")

		ax.plot3D(states[:, 0, 1, 0], states[:, 0, 1, 1], states[:, 0, 1, 2], label="mercury", color="orange")
		it += 1

	initial = np.array([planets.positions, planets.velocities])
	ts, states = parareal(
		simulation, 0, 365, 
		initial, 
		num_segments=365*2, 
		max_iters=6, 
		eps=1e-100, 
		coarse_solver=verlet, fine_solver=rk4,
		observer=plot_one)
	
	plt.tight_layout()
	plt.savefig("fig_2.eps", format="eps")


if __name__ == "__main__":
	fig_2()
