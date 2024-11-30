import matplotlib.pyplot as plt
import numpy as np
import sys


def compare(parareal_outfile, serial_outfile):
	data_1 = np.loadtxt(sys.argv[1])
	data_1, energy_1 = data_1[:, :-2], data_1[:, -2:]
	data_1 = data_1.reshape((data_1.shape[0], data_1.shape[1]//3, 3))

	data_2 = np.loadtxt(sys.argv[2])
	data_2, energy_2 = data_2[:, :-2], data_2[:, -2:]
	data_2 = data_2.reshape((data_2.shape[0], data_2.shape[1]//3, 3))

	data_1 = data_1[:min(len(data_1), len(data_2))]
	data_2 = data_2[:min(len(data_1), len(data_2))]

	abs_errs = np.linalg.norm(data_1-data_2, axis=-1)
	rel_errs = abs_errs/np.linalg.norm(data_2, axis=-1)

	return abs_errs, rel_errs


if __name__ == "__main__":
	if len(sys.argv) <= 2:
		print("not enough arguments", file=sys.stderr)
		print("python3 dist.py <parareal out> <serial out>")
		sys.exit(1)

	abs_errs, rel_errs = compare(sys.argv[1], sys.argv[2])

	print(f"max abs err: {abs_errs.max():.4f} AU")
	print(f"mean abs err: {abs_errs.mean():.4f} AU")

	print(f"max rel err: {rel_errs.max() * 100:.2f} %")
	print(f"mean rel err: {rel_errs.mean() * 100:.2f} %")

	#plt.hist(rel_errs[:, 1])
	#plt.plot(np.arange(len(data_1)), rel_errs[:, 1])
	#plt.show()
