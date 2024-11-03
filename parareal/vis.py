import matplotlib.pyplot as plt
import numpy as np
import sys


if len(sys.argv) <= 1:
	print("not enough arguments", file=sys.stderr)
	sys.exit(1)

data = np.loadtxt(sys.argv[1])
data, energy = data[:, :-2], data[:, -2:]
data = data.reshape((data.shape[0], data.shape[1]//2, 2))

lim = 1.2*max(data[0, :, :].min(), data[0, :, :].max())

fig, ax = plt.subplots(1, 2)
ax[0].set_xlim([-lim, lim])
ax[0].set_ylim([-lim, lim])
particles_plot = ax[0].scatter(data[0, :, 0], data[0, :, 1])

ax[1].plot(np.arange(len(energy)), energy[:, 0])
ax[1].plot(np.arange(len(energy)), energy[:, 1])
ax[1].plot(np.arange(len(energy)), energy.sum(axis=1))

for i in range(len(data)):
	particles_plot.set_offsets(np.c_[data[i, :, 0], data[i, :, 1]])
	plt.pause(0.01)
