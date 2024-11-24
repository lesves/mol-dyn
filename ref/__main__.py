import functools

import matplotlib.pyplot as plt
import numpy as np

from .parareal import parareal, serial_integrate, solver_num_steps, verlet, rk4
from .simulation import simulation
from . import planets


def simulate_parareal(observer=None):
	initial = np.array([planets.positions, planets.velocities])

	coarse_solver = verlet
	fine_solver = functools.partial(solver_num_steps, rk4, 10)

	ts, states = parareal(
		simulation, 0, 365*10, 
		initial, 
		num_segments=10_000, 
		max_iters=1000, 
		eps=1e-1, 
		coarse_solver=coarse_solver, fine_solver=fine_solver,
		observer=observer)

	return states


def simulate_serial(solver=verlet):
	initial = np.array([planets.positions, planets.velocities])

	ts, states = serial_integrate(simulation, 0, 365*10, initial, solver, 10_000)
	return states


def plot(states):
	ax = plt.axes(projection="3d")
	for i, name in enumerate(planets.names):
		ax.plot3D(states[:, 0, i, 0], states[:, 0, i, 1], states[:, 0, i, 2], label=name)
	ax.legend()
	plt.show()


if __name__ == "__main__":
	plot(simulate_parareal(), show=True)
