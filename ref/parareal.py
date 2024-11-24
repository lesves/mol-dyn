import numpy as np
import matplotlib.pyplot as plt
#from loky import get_reusable_executor


def euler(f, t0, t1, u):
	return u + (t1-t0)*f(t1, u)


def verlet(f, t0, t1, u):
	dt = t1-t0

	x, v = u.copy()
	_, a0 = f(t0, u)
	x += v*dt + 1/2*a0*dt**2
	_, a1 = f(t1, u)
	v += (a0+a1)/2 * dt
	return np.array([x, v])


def rk4(f, t0, t1, u):
	dt = t1-t0

	k1 = f(t0       , u          )
	k2 = f(t0 + dt/2, u + dt*k1/2)
	k3 = f(t0 + dt/2, u + dt*k2/2)
	k4 = f(t0 + dt  , u + dt*k3  )
	return u + dt/6*(k1 + 2*k2 + 2*k3 + k4)


def serial_integrate(f, t0, T, u0, solver, num_steps):
	dt = (T-t0)/num_steps

	ts = [t0]
	us = [u0]
	for i in range(1, num_steps+1):
		t = ts[-1]
		u = us[-1]

		u = solver(f, t, t+dt, u)
		t = t+dt

		ts.append(t)
		us.append(u)
	return np.array(ts), np.array(us)


def solver_num_steps(solver, num_steps, f, t0, t1, u):
	return serial_integrate(f, t0, t1, u, solver, num_steps)[1][-1]


def parareal(f, t0, T, u0, num_segments, max_iters, eps, coarse_solver, fine_solver, observer=None):
	segment_size = (T-t0)/num_segments

	#executor = get_reusable_executor()

	# 0th iteration
	ts, us = serial_integrate(f, t0, T, u0, coarse_solver, num_segments)

	for it in range(1, max_iters+1):
		if observer is not None:
			observer(us)

		us_ = [u0]

		#def fine_u(i):
		#	return fine_solver(f, ts[i], ts[i]+segment_size, us[i])

		#fine_us = list(executor.map(fine_u, range(len(us)-1)))

		for i in range(len(us)-1):
			t = ts[i]

			u_ = coarse_solver(f, t, t+segment_size, us_[i]) + fine_solver(f, t, t+segment_size, us[i]) - coarse_solver(f, t, t+segment_size, us[i])
			us_.append(u_)
		us_ = np.array(us_)

		dist = np.abs(us-us_).max()
		print(f"parareal iter={it} dist={dist}")
		if (dist < eps).all():
			us = us_
			break

		us = us_

	return ts, us


def test_problem(show=True):
	# A harmonic oscillator
	def harmonic(t, u, k=1):
		x, v = u
		return np.array([v, -k*x])

	t0 = 0
	T  = 10
	v0 = 1
	x0 = 0

	num_segments = 100

	for max_iters in list(range(0, 3)):
		ts, states = parareal(harmonic, t0, T, np.array([x0, v0]), num_segments, max_iters, 1e-12, euler, rk4)
		xs = states[:, 0]

		plt.plot(ts, xs, label=f"max {max_iters} iters")

	plt.title("Parareal example – Harmonic oscillator")

	plt.plot(ts, np.sin(ts), label=f"true function")

	plt.legend()
	if show:
		plt.show()


if __name__ == "__main__":
	test_problem()
