#ifndef PARAREAL_INTEGRATION_H
#define PARAREAL_INTEGRATION_H

#include <functional>
#include "state.hpp"

namespace integration {
	void euler_step(types::real dt, const config::Config& cfg, state::State& state) {
		state.t += dt;

		for (std::size_t i = 0; i < state.size(); ++i) {
			state.compute_force(i, cfg);
			state.x[i] += state.v[i]*dt;
			state.v[i] += state.a[i]*dt;
		}
	}

	void leapfrog_step(types::real dt, const config::Config& cfg, state::State& state) {
		for (std::size_t i = 0; i < state.size(); ++i) {
			state.v[i] += state.a[i]*0.5*dt;
			state.x[i] += state.v[i]*dt;
			state.compute_force(i, cfg);
		}
		for (std::size_t i = 0; i < state.size(); ++i) {
			state.v[i] += state.a[i]*0.5*dt;
		}
		state.t += dt;
	}

	void rk4_step(types::real dt, const config::Config& cfg, state::State& state) {
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
			tmp.compute_force(i, cfg);

			k1_v = tmp.a[i]*dt;
			k1_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k1_x/(types::real)2;
			tmp.v[i] = state.v[i] + k1_v/(types::real)2;
			tmp.compute_force(i, cfg);

			k2_v = tmp.a[i]*dt;
			k2_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k2_x/(types::real)2;
			tmp.v[i] = state.v[i] + k2_v/(types::real)2;
			tmp.compute_force(i, cfg);

			k3_v = tmp.a[i]*dt;
			k3_x = tmp.v[i]*dt;

			tmp.x[i] = state.x[i] + k3_x;
			tmp.v[i] = state.v[i] + k3_v;
			tmp.compute_force(i, cfg);

			k4_v = tmp.a[i]*dt;
			k4_x = tmp.v[i]*dt;

	        state.v[i] += (k1_v + (types::real)2*k2_v + (types::real)2*k3_v + k4_v)/(types::real)6;
	        state.x[i] += (k1_x + (types::real)2*k2_x + (types::real)2*k3_x + k4_x)/(types::real)6;
		}
	}

	class Integrator {
		using IntegrationStepType = std::function<void(types::real, const config::Config&, state::State&)>;
		IntegrationStepType step_;

		types::real t0, T, dt;
		std::uint64_t num_iters;
		std::uint64_t it;

		const config::Config& cfg;
		state::State state;

	public:
		Integrator(
			IntegrationStepType step_, 
			types::real t0, 
			types::real T, 
			std::uint64_t num_iters,
			const config::Config& cfg,
			state::State& state
		): step_(step_), t0(t0), T(T), num_iters(num_iters), dt((T-t0)/num_iters), cfg(cfg), state(std::move(state)) {}

		const state::State* step() {
			step_(dt, cfg, state);
			it++;
			return &state;
		}

		bool done() const {
			return it >= num_iters;
		}

		std::uint64_t iter() const {
			return it;
		}
	};
}

#endif