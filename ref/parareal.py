import numpy as np
import matplotlib.pyplot as plt
from functools import wraps
from loky import get_reusable_executor


def basic_solver(sol):
	@wraps(sol)
	def wrapper(f, t0, t1, u0, return_last=True, **kwargs):
		u1 = sol(f, t0, t1, u0, **kwargs)
		if return_last:
			return u1
		else:
			return (np.array([t0, t1]), np.array([u0, u1]))
	return wrapper


@basic_solver
def euler(f, t0, t1, u):
	return u + (t1-t0)*f(t1, u)


@basic_solver
def verlet(f, t0, t1, u):
	dt = t1-t0

	x, v = u.copy()
	_, a0 = f(t0, u)
	x += v*dt + 1/2*a0*dt**2
	_, a1 = f(t1, u)
	v += (a0+a1)/2 * dt
	return np.array([x, v])


@basic_solver
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


def solver_num_steps(solver, num_steps, f, t0, t1, u, return_last=True):
	ts, xs = serial_integrate(f, t0, t1, u, solver, num_steps)
	if return_last:
		return xs[-1]
	return ts, xs


def parallel_map(f, data, executor=get_reusable_executor()):
	return executor.map(f, data)


def parareal_parallel_fine(fine_solver, f, ts, segment_size, us, **kwargs):
	def fine_u(i):
		return fine_solver(f, ts[i], ts[i]+segment_size, us[i], **kwargs)

	return parallel_map(fine_u, range(len(us)-1))


def parareal(f, t0, T, u0, num_segments, max_iters, eps, coarse_solver, fine_solver, coarse_f=None, observer=None):
	segment_size = (T-t0)/num_segments
	print(f"{segment_size=}")

	#executor = get_reusable_executor()

	if coarse_f is None:
		coarse_f = f

	# 0th iteration
	ts, us = serial_integrate(coarse_f, t0, T, u0, coarse_solver, num_segments)

	for it in range(1, max_iters+1):
		if observer is not None:
			observer(us)

		us_ = [u0]

		fine_us = list(parareal_parallel_fine(fine_solver, f, ts, segment_size, us))

		for i in range(len(us)-1):
			t = ts[i]

			u_ = coarse_solver(coarse_f, t, t+segment_size, us_[i]) + fine_us[i] - coarse_solver(coarse_f, t, t+segment_size, us[i])
			us_.append(u_)
		us_ = np.array(us_)

		dist = np.abs(us-us_).max()
		print(f"parareal iter={it} dist={dist}")
		if (dist < eps).all():
			us = us_
			break

		us = us_

	res = list(parareal_parallel_fine(fine_solver, f, ts, segment_size, us, return_last=False))
	return np.concatenate([ts for ts, us in res]), np.concatenate([us for ts, us in res])


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

	plt.xlabel("t [s]")
	plt.ylabel("x [m]")

	plt.legend()
	if show:
		plt.show()


if __name__ == "__main__":
	test_problem()
