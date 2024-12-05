import matplotlib.pyplot as plt
import numpy as np
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../ref"))
import planets


if __name__ == "__main__":
	if len(sys.argv) <= 1:
		print("not enough arguments", file=sys.stderr)
		sys.exit(1)

	data = np.loadtxt(sys.argv[1])
	data, energy = data[:, :-2], data[:, -2:]
	data = data.reshape((data.shape[0], data.shape[1]//3, 3))

	for state, saved in zip(data, energy[:, 1]):
		correct = 0
		for i in range(len(state)):
			for j in range(len(state)):
				if i == j:
					continue
					
				dist = np.linalg.norm(state[i] - state[j])

				correct += -planets.G * planets.masses[i] * planets.masses[j]/dist/2

		diff = np.abs(correct-saved)
		print(f"{correct=}, {saved=}, {diff=}")
