#ifndef PARAREAL_SIMULATION_H
#define PARAREAL_SIMULATION_H

namespace parareal { namespace simulation {
    class simulation {
        state::real G;
        state::real smoothing_eps = 0.;
        const state::mass_type& masses;

    public:
        simulation(state::real G, const state::mass_type& masses): G(G), masses(masses) {};

        void operator()(const state::state_type &state, state::state_type &der, const state::real t) const {
            const std::size_t N = state::size(state);

            for (std::size_t i = 0; i < N; ++i) {

                state::clear_vel(der, i);

                for (std::size_t j = 0; j < N; ++j) {
                    if (i == j) {
                        continue;
                    }
                    const auto diff = state::get_pos(state, i)-state::get_pos(state, j);
                    const auto dist = diff.norm();

                    const auto smoothed = std::sqrt(dist*dist + smoothing_eps*smoothing_eps);

                    const auto acc_j = -G * masses[j] * diff / std::pow(smoothed, 3.);
                    state::set_vel(der, i, state::get_vel(der, i) + acc_j);
                }

                state::set_pos(der, i, state::get_vel(state, i));
            }
        }
    };
} }

#endif