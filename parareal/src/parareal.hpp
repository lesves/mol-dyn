#ifndef PARAREAL_PARAREAL_H
#define PARAREAL_PARAREAL_H

#include <boost/numeric/odeint.hpp>
#include <boost/mpi/communicator.hpp>

#include <vector>
#include "state.hpp"

namespace parareal {
    namespace odeint = boost::numeric::odeint;
    namespace mpi = boost::mpi;

    std::vector<state::state_type> parareal(
        mpi::communicator& world,
        auto system,
        state::real t0, state::real T,
        const state::state_type& initial,
        std::uint64_t num_segments,
        std::uint64_t max_iters,
        state::real eps,
        auto coarse_solver,
        auto fine_solver,
        state::real coarse_step_size,
        state::real fine_step_size
    ) {
        const state::real segment_size = (T-t0) / num_segments;
        const std::size_t worker_size = num_segments/world.size();

        if (num_segments % world.size() != 0) {
            throw std::runtime_error("Number of segments must be divisible by the number of ranks");
        }

        std::vector<state::state_type> states = { initial };

        // 0th iteration
        state::real time = 0.;
        auto u = initial;
        for (std::size_t seg = 1; seg < num_segments; ++seg) {
            odeint::integrate_const(coarse_solver, system, u, time, time+segment_size, coarse_step_size);
            states.push_back(u);
            time += segment_size;
        }

        bool stop = false;
        for (std::uint64_t it = 1; it < max_iters+1; ++it) {
            std::vector<state::state_type> states_new = {initial};

            if (world.rank() == 0) {
                std::cout << "parareal iter=" << it << " ";
            }

            // parallel part
            for (std::size_t seg = 0; seg < num_segments-1; ++seg) {
                auto fine_t0 = segment_size * seg;
                auto fine_T = segment_size * (seg+1);
                auto fine_u = states[seg];

                std::cout << seg << " " << fine_t0 << " " << fine_T << " " << fine_step_size << std::endl;

                odeint::integrate_adaptive(fine_solver, system, fine_u, fine_t0, fine_T, fine_step_size);
                states_new.push_back(std::move(fine_u));
            }

            // do updates
            for (std::size_t seg = 0; seg < num_segments-1; ++seg) {
                auto a = states_new[seg];
                odeint::integrate_const(coarse_solver, system, a, seg*segment_size, (seg+1)*segment_size, coarse_step_size);

                auto c = states[seg];
                odeint::integrate_const(coarse_solver, system, c, seg*segment_size, (seg+1)*segment_size, coarse_step_size);

                for (std::size_t i = 0; i < states_new[seg+1].size(); ++i) {
                    states_new[seg+1][i] += a[i] - c[i];
                }
            }

            // check if converged
            auto dist = state::distance(states.back(), states_new.back());
            if (world.rank() == 0) {
                std::cout << "distance=" << dist << std::endl;
            }
            if (dist < eps) {
                stop = true;
            }

            states = std::move(states_new);

            if (stop) {
                break;
            }
        }

        return states;
    }
}

#endif