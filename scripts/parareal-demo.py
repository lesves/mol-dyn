import numpy as np
import matplotlib.pyplot as plt


# Integrators

def euler(f, t, dt, u):
	return u + dt*f(t+dt, u)

def rk4(f, t, dt, u):
	k1 = f(t       , u          )
	k2 = f(t + dt/2, u + dt*k1/2)
	k3 = f(t + dt/2, u + dt*k2/2)
	k4 = f(t + dt  , u + dt*k3  )
	return u + dt/6*(k1 + 2*k2 + 2*k3 + k4)

# Demo parareal

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

def parareal_noparallel(f, t0, T, u0, coarse_solver, fine_solver, num_steps_coarse, num_steps_fine, max_iters, eps):
	dt_coarse = (T-t0)/num_steps_coarse

	# 0th iteration
	ts, us = serial_integrate(f, t0, T, u0, coarse_solver, num_steps_coarse)

	for it in range(1, max_iters+1):
		us_ = [u0]
		for i, (t, u) in enumerate(zip(ts[:-1], us)): # parallel
			t = ts[i]
			u = us[i]
			u_ = us_[i]

			_, fine_segment = serial_integrate(f, t, t+dt_coarse, u, fine_solver, num_steps_fine)
			u_ = coarse_solver(f, t, dt_coarse, u_) + fine_segment[-1] - coarse_solver(f, t, dt_coarse, u)
			us_.append(u_)
		us_ = np.array(us_)

		if (us-us_ < eps).all():
			us = us_
			break

		us = us_

	return ts, us

# Showcase problems

def problem_1():
	# A first order ODE
	def f(t, u):
		return 70*u*np.sin(t)

	u0 = 1
	t0 = 0
	T = 1

	num_steps = 100
	dt = (T-t0)/num_steps

	max_iters = 100
	eps = 1e-5

	_, us_fine = serial_integrate(f, t0, T, u0, rk4, num_steps)
	_, us_coarse = serial_integrate(f, t0, T, u0, euler, num_steps)
	ts, us = parareal_noparallel(f, t0, T, u0, euler, rk4, num_steps, 1, max_iters, eps)

	#plt.plot(ts, [f(t, u) for t, u in zip(ts, us)], label="derivative")
	plt.plot(ts, us, label=f"parareal integration with eps = {eps}")
	plt.plot(ts, us_coarse, label="coarse integration")
	plt.plot(ts, us_fine, label="fine integration")
	#plt.plot(ts, [np.exp(-70*(np.cos(t)-1)) for t in ts], label="true function")
	plt.legend()
	plt.show()

def problem_2():
	# A harmonic oscillator
	def harmonic(t, u, k=1):
		x, v = u
		return np.array([v, -k*x])

	t0 = 0
	T  = 10
	v0 = 1
	x0 = 0

	num_steps_coarse = 100
	num_steps_fine = 1

	max_iters = 1

	_, coarse_states = serial_integrate(harmonic, t0, T, np.array([x0, v0]), euler, num_steps_coarse)
	coarse_xs = coarse_states[:, 0]

	fine_ts, fine_states = serial_integrate(harmonic, t0, T, np.array([x0, v0]), rk4, num_steps_coarse*num_steps_fine)
	fine_xs = fine_states[:, 0]

	ts, states = parareal_noparallel(harmonic, t0, T, np.array([x0, v0]), euler, rk4, num_steps_coarse, num_steps_fine, max_iters, 1e-12)
	xs = states[:, 0]

	plt.plot(ts, coarse_xs, label="coarse integration")
	plt.plot(fine_ts, fine_xs, label="fine integration")
	plt.plot(ts, xs, label=f"parareal integration after at most {max_iters} iterations")
	plt.legend()
	plt.show()


if __name__ == "__main__":
	problem_2()
