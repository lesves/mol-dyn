import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import RK45
import planets
from parareal import parareal, rk45
import functools


def model(shape):
	def decor(f):
		@functools.wraps(f)
		def func(t, y):
			return f(t, y.reshape(shape)).flatten()
		return func
	return decor


initial = np.array([planets.positions, planets.velocities])

@model(initial.shape)
def simulation(t, state):
	positions, velocities = state

	differences = positions[:, None, :] - positions[None, :, :]
	differences = differences.reshape(positions.shape[0]**2, positions.shape[1])

	with np.errstate(divide="ignore", invalid="ignore"):
		diminish = differences / (np.linalg.norm(differences, axis=1)**3)[:, None]

	diminish = diminish.reshape(positions.shape[0], positions.shape[0], positions.shape[1])
	diminish[np.diag_indices(len(diminish))] = np.zeros((positions.shape[1],))

	accelerations = -planets.G * (diminish * planets.masses[:, None]).sum(axis=1)

	return np.array([velocities, accelerations])


def plot(states):
	ax = plt.axes(projection="3d")
	for i, name in enumerate(planets.names):
		ax.plot3D(states[:, 0, i, 0], states[:, 0, i, 1], states[:, 0, i, 2], label=name)
	ax.legend()
	plt.show()


def rk4(f, t, dt, u):
	k1 = f(t       , u          )
	k2 = f(t + dt/2, u + dt*k1/2)
	k3 = f(t + dt/2, u + dt*k2/2)
	k4 = f(t + dt  , u + dt*k3  )
	return u + dt/6*(k1 + 2*k2 + 2*k3 + k4)


def simulate_parareal():
	coarse_solver = rk4
	fine_solver = functools.partial(rk45, rtol=1e-6)
	ts, states = parareal(simulation, 0, 366*100, initial.flatten(), 1000, 1000, 1e-6, coarse_solver=coarse_solver, fine_solver=fine_solver)

	return states.reshape((-1,) + initial.shape)


def simulate_rk45():
	solver = RK45(simulation, 0, initial.flatten(), 366*100, rtol=1e-6)

	ts = []
	states = []
	while solver.status == "running":
		ts.append(solver.t)
		state = solver.y.reshape(initial.shape)
		states.append(state)
		solver.step()

	return np.array(states)


plot(simulate_parareal())
