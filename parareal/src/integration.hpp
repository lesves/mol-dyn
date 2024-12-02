#ifndef PARAREAL_INTEGRATION_H
#define PARAREAL_INTEGRATION_H

#include <vector>
#include <functional>
#include <omp.h>

#include "state.hpp"

namespace parareal { namespace integration {
	class EulerStep {
	public:
		void operator()(double t0, double t1, state::State& state) {
			double dt = t1-t0;

			state.compute_forces();
			for (std::size_t i = 0; i < state.size(); ++i) {
				state.x[i] += state.v[i]*dt;
				state.v[i] += state.a[i]*dt;
			}
		}
	};

	class VelocityVerlet {
	private:
		std::size_t num_steps;

	public:
		VelocityVerlet(std::size_t num_steps): num_steps(num_steps) {};

		void operator()(double t0, double t1, state::State& state) {
			double dt = (t1-t0)/num_steps;
			state.compute_forces();

			for (std::size_t it = 0; it < num_steps; ++it) {
				for (std::size_t i = 0; i < state.size(); ++i) {
					state.x[i] += dt * (state.v[i] + 0.5*state.a[i]*dt);
				}

				for (std::size_t i = 0; i < state.size(); ++i) {
					// assumption: accelerations don't depend on velocities
					const types::vec3 a0 = state.a[i];
					state.compute_force(i);
					const types::vec3 a1 = state.a[i];
					state.v[i] += 0.5*dt*a0 + 0.5*dt*a1;
				}
			}
		}
	};

	class RK4Step {
	public:
		void operator()(double t0, double t1, state::State& state) {
			double dt = t1-t0;

			std::vector<types::vec3> k1_v(state.size());
			std::vector<types::vec3> k2_v(state.size());
			std::vector<types::vec3> k3_v(state.size());
			std::vector<types::vec3> k4_v(state.size());
			std::vector<types::vec3> k1_x(state.size());
			std::vector<types::vec3> k2_x(state.size());
			std::vector<types::vec3> k3_x(state.size());
			std::vector<types::vec3> k4_x(state.size());

			state::State tmp = state;

			tmp.compute_forces();

			for (std::size_t i = 0; i < tmp.size(); ++i) {
				k1_v[i] = tmp.a[i]*dt;
				k1_x[i] = tmp.v[i]*dt;

				tmp.x[i] = state.x[i] + k1_x[i]/2.;
				tmp.v[i] = state.v[i] + k1_v[i]/2.;
			}

			tmp.compute_forces();

			for (std::size_t i = 0; i < tmp.size(); ++i) {
				k2_v[i] = tmp.a[i]*dt;
				k2_x[i] = tmp.v[i]*dt;

				tmp.x[i] = state.x[i] + k2_x[i]/2.;
				tmp.v[i] = state.v[i] + k2_v[i]/2.;
			}

			tmp.compute_forces();

			for (std::size_t i = 0; i < tmp.size(); ++i) {
				k3_v[i] = tmp.a[i]*dt;
				k3_x[i] = tmp.v[i]*dt;

				tmp.x[i] = state.x[i] + k3_x[i];
				tmp.v[i] = state.v[i] + k3_v[i];
			}

			tmp.compute_forces();

			for (std::size_t i = 0; i < tmp.size(); ++i) {
				k4_v[i] = tmp.a[i]*dt;
				k4_x[i] = tmp.v[i]*dt;

		        state.v[i] += (k1_v[i] + 2.*k2_v[i] + 2.*k3_v[i] + k4_v[i])/6.;
		        state.x[i] += (k1_x[i] + 2.*k2_x[i] + 2.*k3_x[i] + k4_x[i])/6.;
			}
		}
	};

	template<typename Step>
	class FixedStepSolver {
	private:
		std::size_t num_steps;
		Step step;

	public:
		FixedStepSolver(std::size_t num_steps): num_steps(num_steps) {};
		FixedStepSolver(std::size_t num_steps, const Step& step): num_steps(num_steps), step(step) {};

		void operator()(double t0, double t1, state::State& state) {
			double dt = (t1-t0) / num_steps;

			for (auto i = 0; i < num_steps; ++i) {
				step(t0 + dt*i, t0 + dt*(i+1), state);
			}
		}

		std::vector<state::State> integrate_verbose(double t0, double t1, state::State& state) {
			double dt = (t1-t0) / num_steps;
			std::vector<state::State> res;

			for (auto i = 0; i < num_steps; ++i) {
				step(t0 + dt*i, t0 + dt*(i+1), state);
				res.push_back(state);
			}

			return res;
		}
	};

	using RK4Fixed = FixedStepSolver<RK4Step>;

	template<typename FixedStepSolverType>
	FixedStepSolver<FixedStepSolverType> solver_with_log_period(std::size_t num_steps, std::size_t log_period) {
		if (num_steps % log_period) {
			throw std::runtime_error("num_steps must be divisible by log_period");
		}
		auto inner = FixedStepSolverType(log_period);
		auto num_steps_ = num_steps/log_period;
		return FixedStepSolver<FixedStepSolverType>(num_steps_, inner);
	}

	template<typename CoarseSolver, typename FineSolver>
	std::vector<state::State> parareal(
		//mpi::communicator& world,
		double t0, double T, 
		const state::State& initial, 
		std::uint64_t num_segments, 
		std::uint64_t num_iters, 
		double eps,
		CoarseSolver coarse_solver,
		FineSolver fine_solver
	) {
		double segment_size = (T-t0)/num_segments;
		std::size_t num_particles = initial.size();

		// 0th iteration, only done by rank 0
		std::vector<state::State> states = {initial};

		auto u = initial;
		for (std::size_t seg = 1; seg < num_segments+1; ++seg) {
			coarse_solver((seg-1)*segment_size, seg*segment_size, u);
			states.push_back(u);
		}

		// main algorithm
		for (std::uint64_t it = 1; it < num_iters+1; ++it) {
			std::vector<state::State> states_new = states;

			// parallel
			#pragma omp parallel for
			for (std::size_t seg = 0; seg < num_segments; ++seg) {
				states_new[seg+1] = states[seg];
				fine_solver(seg*segment_size, (seg+1)*segment_size, states_new[seg+1]);
			}

			// serial again - we adjust the results
			for (std::size_t seg = 0; seg < num_segments; ++seg) {
				auto a = states_new[seg];
				coarse_solver(seg*segment_size, (seg+1)*segment_size, a);
				auto c = states[seg];
				coarse_solver(seg*segment_size, (seg+1)*segment_size, c);

				for (std::size_t i = 0; i < states_new[seg+1].size(); ++i) {
					states_new[seg+1].x[i] += a.x[i] - c.x[i];
					states_new[seg+1].v[i] += a.v[i] - c.v[i];
				}
			}

			// check if converged
			double max_dist = 0;
			for (std::size_t i = 0; i < states.size(); ++i) {
				auto dist = states[i].distance(states_new[i]);
				if (dist > max_dist) {
					max_dist = dist;
				}
			}
			
			std::cout << "distance=" << max_dist << std::endl;
			if (max_dist < eps) {
				break;
			}

			states = std::move(states_new);
		}

		std::vector<std::vector<state::State>> results(num_segments);

		// parallel
		#pragma omp parallel for
		for (std::size_t seg = 0; seg < num_segments; ++seg) {
			results[seg] = fine_solver.integrate_verbose(seg*segment_size, (seg+1)*segment_size, states[seg]);
		}

		std::vector res = { initial };
		for (auto&& r : results) {
			res.insert(res.end(), r.begin(), r.end());
		}

		return res;
	}
} }

#endif