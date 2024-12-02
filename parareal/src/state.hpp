#ifndef PARAREAL_STATE_H
#define PARAREAL_STATE_H

#include <algorithm>
#include <vector>
#include <cmath>

#include "types.hpp"

//#define SMOOTHING_EPS 0


namespace parareal { namespace state {
	class Config {
	public:
		double G = 1;
		std::vector<double> m;

		Config(double G, const std::vector<double>& m): G(G), m(m) {};
		Config(double G, const std::vector<double>&& m): G(G), m(std::move(m)) {};
	};

	class State {
	private:
		const Config* cfg;

	public:
		std::vector<types::vec3> x;
		std::vector<types::vec3> v;
		std::vector<types::vec3> a;

		State(
			const Config* cfg_,
			const std::vector<types::vec3>& x_,
			const std::vector<types::vec3>& v_
		): cfg(cfg_), x(x_), v(v_), a(x.size()) {}

		State(
			const Config* cfg_,
			std::vector<types::vec3>&& x_,
			std::vector<types::vec3>&& v_
		): cfg(cfg_), x(std::move(x_)), v(std::move(v_)), a(x.size()) {}

		std::size_t size() const {
			return x.size();
		}

		void compute_forces() {
			for (std::size_t i = 0; i < size(); ++i) {
				compute_force(i);
			}
		}

		void compute_force(std::size_t i) {
			a[i].clear();

			for (std::size_t j = 0; j < size(); ++j) {
				if (i == j) {
					continue;
				}

				auto diff = x[i]-x[j];
				auto dist = diff.norm();

				//auto smoothed = std::sqrt(dist*dist + SMOOTHING_EPS*SMOOTHING_EPS);

				a[i] += -cfg->G * cfg->m[j] * diff / std::pow(dist, 3.);
			}
		}

		double potential_energy() const {
			double energy;

			for (std::size_t i = 0; i < size(); ++i) {
				for (std::size_t j = 0; j < size(); ++j) {
					if (i == j) {
						continue;
					}

					auto diff = x[i]-x[j];
					auto dist = diff.norm();

					//auto smoothed = std::sqrt(dist*dist + SMOOTHING_EPS*SMOOTHING_EPS);

					energy += -cfg->G * cfg->m[i] * cfg->m[j] / dist / 2.;
				}
			}

			return energy;
		}

		double kinetic_energy() const {
			double energy;

			for (std::size_t i = 0; i < size(); ++i) {
				energy += 0.5 * cfg->m[i]*v[i].norm_squared();
			}

			return energy;
		}

		double distance(const State& other) const {
			double diff = 0;

			for (std::size_t i = 0; i < size(); ++i) {
				diff = std::max<double>(diff, (x[i]-other.x[i]).norm());
				diff = std::max<double>(diff, (v[i]-other.v[i]).norm());
			}

			return diff;
		}

		// dump to arrays for MPI
		void serialize(double* state_xs, double* state_vs) const {
			for (std::size_t i = 0; i < size(); ++i) {
				for (std::size_t d = 0; d < 3; ++d) {
					state_xs[d+i*size()] = x[i][d];
					state_vs[d+i*size()] = v[i][d];
				}
			}
		}

		// replaces the particles inside with given particles, must be the same size!
		void deserialize(const double* state_xs, const double* state_vs) {
			for (std::size_t i = 0; i < size(); ++i) {
				for (std::size_t d = 0; d < 3; ++d) {
					x[i][d] = state_xs[d+i*size()];
					v[i][d] = state_vs[d+i*size()];
				}
			}
		}
	};
} }


#endif