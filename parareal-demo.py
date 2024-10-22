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

def serial_integrate(f, t0, u0, solver, num_steps, dt):
	ts = [t0]
	us = [u0]
	for i in range(1, num_steps):
		t = ts[-1]
		u = us[-1]

		u = solver(f, t, dt, u)
		t = t+dt

		ts.append(t)
		us.append(u)
	return np.array(ts), np.array(us)

def parareal_noparallel(f, t0, u0, coarse_solver, fine_solver, num_steps, dt, max_iters, eps):
	# 0th iteration
	ts, us = serial_integrate(f, t0, u0, coarse_solver, num_steps, dt)

	for it in range(1, max_iters):
		us_ = [u0]
		for i, (t, u) in enumerate(zip(ts[:-1], us)): # parallel
			t = ts[i]
			u = us[i]
			u_ = us_[i]

			u_ = coarse_solver(f, t, dt, u_) + fine_solver(f, t, dt, u) - coarse_solver(f, t, dt, u)
			us_.append(u_)
		us_ = np.array(us_)

		if (us-us_ < eps).all():
			us = us_
			break

		us = us_

	return ts, us

# Problem

def f(t, u):
	return 70*u*np.sin(t)

u0 = 1
t0 = 0
T = 1

num_steps = 100
dt = (T-t0)/num_steps

max_iters = 100
eps = 1e-5

_, us_fine = serial_integrate(f, t0, u0, rk4, num_steps, dt)
_, us_coarse = serial_integrate(f, t0, u0, euler, num_steps, dt)
ts, us = parareal_noparallel(f, t0, u0, euler, rk4, num_steps, dt, max_iters, eps)

plt.plot(ts, us, label="parareal integration")
plt.plot(ts, us_coarse, label="coarse integration")
plt.plot(ts, us_fine, label="fine integration")
#plt.plot(ts, [np.exp(-70*(np.cos(t)-1)) for t in ts], label="true function")
plt.legend()
plt.show()
