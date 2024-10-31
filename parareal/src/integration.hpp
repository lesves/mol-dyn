#ifndef PARAREAL_INTEGRATION_H
#define PARAREAL_INTEGRATION_H

#include <vector>
#include <functional>
#include "state.hpp"

namespace integration {
	void euler_step(types::real dt, state::State& state) {
		state.t += dt;

		for (std::size_t i = 0; i < state.size(); ++i) {
			state.compute_force(i);
			state.x[i] += state.v[i]*dt;
			state.v[i] += state.a[i]*dt;
		}
	}

	void leapfrog_step(types::real dt, state::State& state) {
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

	void rk4_step(types::real dt, state::State& state) {
		types::vec k1_v;
		types::vec k2_v;
		types::vec k3_v;
		types::vec k4_v;
		types::vec k1_x;
		types::vec k2_x;
		types::vec k3_x;
		types::vec k4_x;

		state::State tmp = state;

		tmp.t += dt/2;
		for (std::size_t i = 0; i < tmp.size(); ++i) {
			tmp.compute_force(i);

			k1_v = tmp.a[i]*dt;
			k1_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k1_x/(types::real)2;
			tmp.v[i] = state.v[i] + k1_v/(types::real)2;
			tmp.compute_force(i);

			k2_v = tmp.a[i]*dt;
			k2_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k2_x/(types::real)2;
			tmp.v[i] = state.v[i] + k2_v/(types::real)2;
			tmp.compute_force(i);

			k3_v = tmp.a[i]*dt;
			k3_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k3_x;
			tmp.v[i] = state.v[i] + k3_v;
			tmp.compute_force(i);

			k4_v = tmp.a[i]*dt;
			k4_x = tmp.v[i]*dt;

	        state.v[i] += (k1_v + (types::real)2*k2_v + (types::real)2*k3_v + k4_v)/(types::real)6;
	        state.x[i] += (k1_x + (types::real)2*k2_x + (types::real)2*k3_x + k4_x)/(types::real)6;
		}
	}

	std::vector<state::State> parareal(
		types::real t0, types::real T, 
		const state::State& initial, 
		std::uint64_t num_segments, 
		std::uint64_t num_iters, 
		types::real eps,
		std::function<void(types::real, state::State&)> coarse_solver = euler_step
	) {
		types::real segment_size = (T-t0)/num_segments;

		// 0th iteration
		std::vector<state::State> states = {initial};
		auto u = initial;
		for (std::size_t seg = 1; seg < num_segments+1; ++seg) {
			coarse_solver(segment_size, u);
			states.push_back(u);
		}

		// main algorithm
		for (std::uint64_t it = 1; it < num_iters+1; ++it) {
			std::cout << "parareal iter=" << it << " ";
			std::vector<state::State> states_new = {initial};

			for (std::size_t seg = 0; seg < num_segments; ++seg) { // parallel
				auto fine_t0 = segment_size * (types::real)seg;
				auto fine_T = segment_size * (types::real)(seg+1);
				auto fine_u = states[seg];

				// fine solver part
				// TODO: RK45
				constexpr std::uint64_t fine_num_iters = 100;
				auto fine_dt = (fine_T-fine_t0)/(types::real)fine_num_iters;
				for (auto i = 0; i < fine_num_iters; ++i) {
					rk4_step(fine_dt, fine_u);
				}
				// end of fine solver part

				auto a = states_new[seg];
				coarse_solver(segment_size, a);
				auto c = states[seg];
				coarse_solver(segment_size, c);

				for (std::size_t i = 0; i < fine_u.size(); ++i) {
					fine_u.x[i] += a.x[i] - c.x[i];
					fine_u.v[i] += a.v[i] - c.v[i];
				}
				states_new.push_back(fine_u);
			}

			// check if converged
			auto dist = states.back().distance(states_new.back());
			std::cout << "distance=" << dist << std::endl;
			if (dist < eps) {
				break;
			}

			states = std::move(states_new);
		}

		return states;
	}
}

#endif