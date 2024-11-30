import numpy as np
import sys


if len(sys.argv) <= 2:
	print("not enough arguments", file=sys.stderr)
	sys.exit(1)

data_1 = np.loadtxt(sys.argv[1])
data_1, energy_1 = data_1[:, :-2], data_1[:, -2:]
data_1 = data_1.reshape((data_1.shape[0], data_1.shape[1]//3, 3))

data_2 = np.loadtxt(sys.argv[2])
data_2, energy_2 = data_2[:, :-2], data_2[:, -2:]
data_2 = data_2.reshape((data_2.shape[0], data_2.shape[1]//3, 3))

data_1 = data_1[:min(len(data_1), len(data_2))]
data_2 = data_2[:min(len(data_1), len(data_2))]

print(np.abs(data_1-data_2).max())
