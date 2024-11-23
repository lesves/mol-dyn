#ifndef PARAREAL_STATE_H
#define PARAREAL_STATE_H

#include <vector>
#include <cmath>
#include "vec.hpp"

namespace parareal { namespace state {
    using real = double;
    using vec = parareal::Vector<double, 3>;

    using state_type = std::vector<double>;
    using mass_type = std::vector<real>;


    std::size_t size(const state_type &state) {
        return state.size()/2/3;
    }

    vec get_pos(const state_type &state, std::size_t i) {
        return vec({state[3*i + 0], state[3*i + 1], state[3*i + 2]});
    }

    vec get_vel(const state_type &state, std::size_t i) {
        const std::size_t N = size(state);
        return vec({state[3*N + 3*i + 0], state[3*N + 3*i + 1], state[3*N + 3*i + 2]});
    }

    void set_pos(state_type &state, std::size_t i, vec val) {
        state[3*i + 0] = val[0];
        state[3*i + 1] = val[1];
        state[3*i + 2] = val[2];
    }

    void set_vel(state_type &state, std::size_t i, vec val) {
        const std::size_t N = size(state);
        state[3*N + 3*i + 0] = val[0];
        state[3*N + 3*i + 1] = val[1];
        state[3*N + 3*i + 2] = val[2];
    }

    void clear_pos(state_type &state, std::size_t i) {
        state[3*i + 0] = 0.;
        state[3*i + 1] = 0.;
        state[3*i + 2] = 0.;
    }

    void clear_vel(state_type &state, std::size_t i) {
        const std::size_t N = size(state);
        state[3*N + 3*i + 0] = 0.;
        state[3*N + 3*i + 1] = 0.;
        state[3*N + 3*i + 2] = 0.;
    }

    real distance(const state_type& a, const state_type& b) {
        real res;

        for (std::size_t i = 0; i < a.size(); ++i) {
            res = std::max<real>(res, std::abs(a[i]-b[i]));
        }

        return res;
    }
} }

#endif
