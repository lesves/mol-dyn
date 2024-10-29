import matplotlib.pyplot as plt
import numpy as np
import sys


if len(sys.argv) <= 1:
	print("not enough arguments", file=sys.stderr)
	sys.exit(1)

data = np.loadtxt(sys.argv[1])
data = data.reshape((data.shape[0], data.shape[1]//2, 2))

lim = 1.2*max(data[0, :, :].min(), data[0, :, :].max())

for i in range(len(data)):
	plt.cla()
	plt.xlim([-lim, lim])
	plt.ylim([-lim, lim])
	plt.scatter(data[i, :, 0], data[i, :, 1])
	plt.pause(0.01)
