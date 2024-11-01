#ifndef PARAREAL_STATE_H
#define PARAREAL_STATE_H

#include <algorithm>
#include <vector>

#include "types.hpp"

#define G 1
#define SMOOTHING_EPS 0


namespace state {
	struct State {
		double t;
		std::vector<types::vec3> x;
		std::vector<types::vec3> v;
		std::vector<types::vec3> a;
		std::vector<double> m;

		State() {}

		void add_particle(types::vec3 pos, types::vec3 vel, double mass) {
			x.push_back(pos);
			v.push_back(vel);
			m.push_back(mass);
			a.push_back(types::vec3());
		}

		std::size_t size() const {
			return x.size();
		}

		void compute_force(std::size_t i) {
			a[i].clear();

			for (std::size_t j = 0; j < size(); ++j) {
				if (i == j) {
					continue;
				}

				auto diff = x[i]-x[j];
				auto dist = diff.norm();

				auto smoothed = std::sqrt(dist*dist + SMOOTHING_EPS*SMOOTHING_EPS);

				a[i] += -G * m[j] * diff / std::pow(smoothed, 3.);
				//pot[i] = -G * m[i] * m[j] / smoothed / (double)2;
			}
		}

		double distance(const State& other) {
			double diff = 0;

			for (std::size_t i = 0; i < size(); ++i) {
				diff = std::max<double>(diff, (x[i]-other.x[i]).norm());
				diff = std::max<double>(diff, (v[i]-other.v[i]).norm());
			}

			return diff;
		}

		// dump to arrays for MPI
		void serialize(double* state_xs, double* state_vs, double* state_ms) {
			for (std::size_t i = 0; i < size(); ++i) {
				for (std::size_t d = 0; d < 3; ++d) {
					state_xs[d+i*size()] = x[i][d];
					state_vs[d+i*size()] = v[i][d];
				}
				state_ms[i] = m[i];
			}
		}

		// replaces the particles inside with given particles, must be the same size!
		void deserialize(double* state_xs, double* state_vs, double* state_ms) {
			for (std::size_t i = 0; i < size(); ++i) {
				for (std::size_t d = 0; d < 3; ++d) {
					x[i][d] = state_xs[d+i*size()];
					v[i][d] = state_vs[d+i*size()];
				}
				m[i] = state_ms[i];
			}
		}
	};
}


#endif