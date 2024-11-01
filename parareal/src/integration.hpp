#ifndef PARAREAL_INTEGRATION_H
#define PARAREAL_INTEGRATION_H

#include <vector>
#include <functional>
#include <mpi.h>

#include "state.hpp"

namespace integration {
	void euler_step(double dt, state::State& state) {
		state.t += dt;

		for (std::size_t i = 0; i < state.size(); ++i) {
			state.compute_force(i);
			state.x[i] += state.v[i]*dt;
			state.v[i] += state.a[i]*dt;
		}
	}

	void leapfrog_step(double dt, state::State& state) {
		for (std::size_t i = 0; i < state.size(); ++i) {
			state.v[i] += state.a[i]*0.5*dt;
			state.x[i] += state.v[i]*dt;
			state.compute_force(i);
		}
		for (std::size_t i = 0; i < state.size(); ++i) {
			state.v[i] += state.a[i]*0.5*dt;
		}
		state.t += dt;
	}

	void rk4_step(double dt, state::State& state) {
		types::vec3 k1_v;
		types::vec3 k2_v;
		types::vec3 k3_v;
		types::vec3 k4_v;
		types::vec3 k1_x;
		types::vec3 k2_x;
		types::vec3 k3_x;
		types::vec3 k4_x;

		state::State tmp = state;

		tmp.t += dt/2;
		for (std::size_t i = 0; i < tmp.size(); ++i) {
			tmp.compute_force(i);

			k1_v = tmp.a[i]*dt;
			k1_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k1_x/2.;
			tmp.v[i] = state.v[i] + k1_v/2.;
			tmp.compute_force(i);

			k2_v = tmp.a[i]*dt;
			k2_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k2_x/2.;
			tmp.v[i] = state.v[i] + k2_v/2.;
			tmp.compute_force(i);

			k3_v = tmp.a[i]*dt;
			k3_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k3_x;
			tmp.v[i] = state.v[i] + k3_v;
			tmp.compute_force(i);

			k4_v = tmp.a[i]*dt;
			k4_x = tmp.v[i]*dt;

	        state.v[i] += (k1_v + 2.*k2_v + 2.*k3_v + k4_v)/6.;
	        state.x[i] += (k1_x + 2.*k2_x + 2.*k3_x + k4_x)/6.;
		}
	}

	void parareal_fine(double fine_t0, double fine_T, state::State& fine_u) {
		// fine solver part
		// TODO: RK45
		constexpr std::uint64_t fine_num_iters = 100;
		auto fine_dt = (fine_T-fine_t0)/(double)fine_num_iters;
		for (auto i = 0; i < fine_num_iters; ++i) {
			rk4_step(fine_dt, fine_u);
		}
		// end of fine solver part
	}

	std::vector<state::State> parareal(
		int rank, int n_ranks,
		double t0, double T, 
		const state::State& initial, 
		std::uint64_t num_segments, 
		std::uint64_t num_iters, 
		double eps,
		std::function<void(double, state::State&)> coarse_solver = euler_step
	) {
		// compute basic segment information
		if (num_segments % n_ranks != 0) {
			throw std::runtime_error("Number of segments must be divisible by the number of ranks");
		}

		double segment_size = (T-t0)/num_segments;
		std::size_t num_particles = initial.size();

		auto worker_size = num_segments/n_ranks;

		// storage for communication between ranks

		double* t0s = new double[num_segments];
		double* Ts = new double[num_segments];
		double* state_xs = new double[num_segments*num_particles*3];
		double* state_vs = new double[num_segments*num_particles*3];
		double* state_ms = new double[num_segments*num_particles];

		double* worker_t0s = new double[worker_size];
		double* worker_Ts = new double[worker_size];
		double* worker_state_xs = new double[worker_size*num_particles*3];
		double* worker_state_vs = new double[worker_size*num_particles*3];
		double* worker_state_ms = new double[worker_size*num_particles];
		state::State worker_state = initial;

		// 0th iteration, only done by rank 0
		std::vector<state::State> states = {initial};
		if (rank == 0) {
			auto u = initial;
			for (std::size_t seg = 1; seg < num_segments+1; ++seg) {
				coarse_solver(segment_size, u);
				states.push_back(u);
			}
		}

		// main algorithm
		for (std::uint64_t it = 1; it < num_iters+1; ++it) {
			std::cout << "parareal iter=" << it << " ";
			std::vector<state::State> states_new = {initial};

			// encode data for sending to workers

			if (rank == 0) {
				for (std::size_t seg = 0; seg < num_segments; ++seg) {
					auto fine_t0 = segment_size * (double)seg;
					auto fine_T = segment_size * (double)(seg+1);
					auto fine_u0 = states[seg];

					t0s[seg] = fine_t0;
					Ts[seg] = fine_T;
					fine_u0.serialize(state_xs+seg*num_particles*3, state_vs+seg*num_particles*3, state_ms+seg*num_particles);
				}
			}

			// send data to workers

			MPI_Scatter(t0s, worker_size, MPI_DOUBLE, worker_t0s, worker_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			MPI_Scatter(Ts, worker_size, MPI_DOUBLE, worker_Ts, worker_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			MPI_Scatter(state_xs, worker_size*num_particles*3, MPI_DOUBLE, worker_state_xs, worker_size*num_particles*3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			MPI_Scatter(state_vs, worker_size*num_particles*3, MPI_DOUBLE, worker_state_vs, worker_size*num_particles*3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			MPI_Scatter(state_ms, worker_size*num_particles, MPI_DOUBLE, worker_state_ms, worker_size*num_particles, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			// individual worker

			for (std::size_t worker_seg = 0; worker_seg < worker_size; ++worker_seg) {
				worker_state.deserialize(worker_state_xs + worker_seg*num_particles*3, worker_state_vs + worker_seg*num_particles*3, worker_state_ms + worker_seg*num_particles);

				parareal_fine(worker_t0s[worker_seg], worker_Ts[worker_seg], worker_state);

				worker_state.serialize(worker_state_xs + worker_seg*num_particles*3, worker_state_vs + worker_seg*num_particles*3, worker_state_ms + worker_seg*num_particles);
			}

			// receive data from workers

			MPI_Gather(worker_state_xs, worker_size*num_particles*3, MPI_DOUBLE, state_xs, worker_size*num_particles*3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			MPI_Gather(worker_state_vs, worker_size*num_particles*3, MPI_DOUBLE, state_vs, worker_size*num_particles*3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			MPI_Gather(worker_state_ms, worker_size*num_particles, MPI_DOUBLE, state_ms, worker_size*num_particles, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			// decode data received from workers

			if (rank == 0) {
				for (std::size_t seg = 0; seg < num_segments; ++seg) {
					auto fine_u = states[seg];

					fine_u.deserialize(state_xs+seg*num_particles*3, state_vs+seg*num_particles*3, state_ms+seg*num_particles);
					states_new.push_back(std::move(fine_u));
				}
			}

			// serial again - we adjust the results
			if (rank == 0) {
				for (std::size_t seg = 0; seg < num_segments; ++seg) {
					auto a = states_new[seg];
					coarse_solver(segment_size, a);
					auto c = states[seg];
					coarse_solver(segment_size, c);

					for (std::size_t i = 0; i < states_new[seg+1].size(); ++i) {
						states_new[seg+1].x[i] += a.x[i] - c.x[i];
						states_new[seg+1].v[i] += a.v[i] - c.v[i];
					}
				}

				// check if converged
				auto dist = states.back().distance(states_new.back());
				std::cout << "distance=" << dist << std::endl;
				if (dist < eps) {
					break;
				}

				states = std::move(states_new);
			}
		}

		// deallocate
		delete[] t0s;
		delete[] Ts;
		delete[] state_xs;
		delete[] state_vs;
		delete[] state_ms;

		delete[] worker_t0s;
		delete[] worker_Ts;
		delete[] worker_state_xs;
		delete[] worker_state_vs;
		delete[] worker_state_ms;

		return states;
	}
}

#endif