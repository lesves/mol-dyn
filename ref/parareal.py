import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import RK45
from loky import get_reusable_executor


def euler(f, t, dt, u):
	return u + dt*f(t+dt, u)


def serial_integrate(f, t0, T, u0, solver, num_steps):
	dt = (T-t0)/num_steps

	ts = [t0]
	us = [u0]
	for i in range(1, num_steps+1):
		t = ts[-1]
		u = us[-1]

		u = solver(f, t, dt, u)
		t = t+dt

		ts.append(t)
		us.append(u)
	return np.array(ts), np.array(us)


def rk45(f, t0, T, u, **kwargs):
	solver = RK45(f, t0, u, T, **kwargs)

	while solver.status == "running":
		solver.step()

	return solver.y


def parareal(f, t0, T, u0, num_segments, max_iters, eps, coarse_solver=euler, fine_solver=rk45):
	segment_size = (T-t0)/num_segments

	executor = get_reusable_executor()

	# 0th iteration
	ts, us = serial_integrate(f, t0, T, u0, coarse_solver, num_segments)

	for it in range(1, max_iters+1):
		us_ = [u0]

		def fine_u(i):
			return fine_solver(f, ts[i], ts[i]+segment_size, us[i])

		fine_us = list(executor.map(fine_u, range(len(us)-1)))

		for i in range(len(us)-1):
			t = ts[i]
			u = us[i]
			u_ = us_[i]
			fine_u = fine_us[i]

			u_ = coarse_solver(f, t, segment_size, u_) + fine_u - coarse_solver(f, t, segment_size, u)
			us_.append(u_)
		us_ = np.array(us_)

		dist = np.abs(us-us_).max()
		print(f"parareal iter={it} dist={dist}")
		if (dist < eps).all():
			us = us_
			break

		us = us_

	return ts, us


def test_problem():
	# A harmonic oscillator
	def harmonic(t, u, k=1):
		x, v = u
		return np.array([v, -k*x])

	t0 = 0
	T  = 10
	v0 = 1
	x0 = 0

	num_segments = 100

	for max_iters in range(0, 3):
		ts, states = parareal(harmonic, t0, T, np.array([x0, v0]), num_segments, max_iters, 1e-12)
		xs = states[:, 0]

		plt.plot(ts, xs, label=f"max {max_iters} iters")

	plt.plot(ts, np.sin(ts), label=f"true function")
	plt.legend()
	plt.show()


if __name__ == "__main__":
	test_problem()
