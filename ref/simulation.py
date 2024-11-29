from numba import njit
import numpy as np
from . import planets


def simulation_array_based(t, state, masses=planets.masses):
	positions, velocities = state

	differences = positions[:, None, :] - positions[None, :, :]
	differences = differences.reshape(positions.shape[0]**2, positions.shape[1])

	with np.errstate(divide="ignore", invalid="ignore"):
		diminish = differences / (np.linalg.norm(differences, axis=1)**3)[:, None]

	diminish = diminish.reshape(positions.shape[0], positions.shape[0], positions.shape[1])
	diminish[np.diag_indices(len(diminish))] = np.zeros((positions.shape[1],))

	accelerations = -planets.G * (diminish * masses[:, None]).sum(axis=1)

	return np.array([velocities, accelerations])


@njit
def simulation(t, state, masses=planets.masses):
	positions, velocities = state

	accelerations = np.zeros_like(positions)
	for i in range(positions.shape[0]):
		for j in range(positions.shape[1]):
			if i == j:
				continue
			diff = positions[i]-positions[j]
			dist = np.sqrt(diff[0]**2 + diff[1]**2 + diff[2]**2)

			accelerations[i] += -planets.G * masses[j] * diff/dist**3

	res = np.zeros_like(state)
	res[0] = velocities
	res[1] = accelerations
	return res


@njit
def coarse_simulation(t, state, masses=planets.masses):
	positions, velocities = state

	accelerations = np.zeros_like(positions)

	for i in range(1, len(masses)):
		diff = positions[i]-positions[0]
		dist = np.sqrt(diff[0]**2 + diff[1]**2 + diff[2]**2)

		accelerations[i] += -planets.G * masses[0] * diff/dist**3

	res = np.zeros_like(state)
	res[0] = velocities
	res[1] = accelerations
	return res
