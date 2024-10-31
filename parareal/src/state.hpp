#ifndef PARAREAL_STATE_H
#define PARAREAL_STATE_H

#include <algorithm>
#include <vector>

#include "types.hpp"

#define G 1
#define SMOOTHING_EPS 0


namespace state {
	struct State {
		types::real t;
		std::vector<types::vec> x;
		std::vector<types::vec> v;
		std::vector<types::vec> a;
		std::vector<types::real> m;

		State() {}

		void add_particle(types::vec pos, types::vec vel, types::real mass) {
			x.push_back(pos);
			v.push_back(vel);
			m.push_back(mass);
			a.push_back(types::vec());
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

				a[i] += -G * m[j] * diff / std::pow(smoothed, (types::real)3);
				//pot[i] = -G * m[i] * m[j] / smoothed / (types::real)2;
			}
		}

		types::real distance(const State& other) {
			types::real diff = 0;

			for (std::size_t i = 0; i < size(); ++i) {
				diff = std::max<types::real>(diff, (x[i]-other.x[i]).norm());
				diff = std::max<types::real>(diff, (v[i]-other.v[i]).norm());
			}

			return diff;
		}
	};
}


#endif