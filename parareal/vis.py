import matplotlib.pyplot as plt
import numpy as np
import sys


if len(sys.argv) <= 1:
	print("not enough arguments", file=sys.stderr)
	sys.exit(1)

data = np.loadtxt(sys.argv[1])
#data, energy = data[:, :-2], data[:, -2:]

data = data.reshape((data.shape[0], data.shape[1]//3, 3))

lim = 1.2*max(data[0, :, :].min(), data[0, :, :].max())

ax = plt.axes(projection="3d")
#ax.set_xlim3d(-lim, lim)
#ax.set_ylim3d(-lim, lim)
#ax.set_zlim3d(-lim, lim)

names = ["sun", "mercury", "venus", "earth", "mars", "jupiter", "saturn", "uranus", "neptune"]

for i, name in zip(range(data.shape[1]), names):
	ax.plot3D(data[:, i, 0], data[:, i, 1], data[:, i, 2], label=name)
ax.legend()
plt.show()

#ax = plt.axes()
#ax.plot(np.arange(len(energy)), energy[:, 0], label="kinetic energy")
#ax.plot(np.arange(len(energy)), energy[:, 1], label="potential energy")
#ax.plot(np.arange(len(energy)), energy.sum(axis=1), label="total energy")
#ax.legend()
#plt.show()

ax = plt.axes()
ax.set_xlim([-lim, lim])
ax.set_ylim([-lim, lim])
particles_plot = ax.scatter(data[0, :, 0], data[0, :, 1])

for i in range(len(data)):
	particles_plot.set_offsets(np.c_[data[i, :, 0], data[i, :, 1]])
	plt.pause(0.01)
