#ifndef PARAREAL_IO_H
#define PARAREAL_IO_H

#include <vector>
#include <tuple>
#include <fstream>
#include "state.hpp"

namespace parareal { namespace io {
    void output(const state::state_type& state, std::ofstream& out) {
        for (std::size_t i = 0; i < state::size(state); ++i) {
            out << state[3*i + 0] << " " << state[3*i + 1] << " " << state[3*i + 2] << " ";
        }
        out << "\n";
    }

    std::tuple<std::vector<std::string>, state::state_type, state::mass_type, state::real> load(const std::string& filename) {
        std::vector<std::string> names;
        std::vector<state::real> pos;
        std::vector<state::real> vel;
        std::vector<state::real> mass;

        std::ifstream in(filename);

        std::string line;
        std::getline(in, line); // take units + constants
        const state::real G = std::stod(line.substr(line.find("G=")+2, -1));
        std::getline(in, line); // discard header

        while (std::getline(in, line)) { // read data
            std::size_t idx = 0;

            for (auto nxt = line.find(','); nxt != -1; nxt = line.find(',')) {
                std::string data = line.substr(0, nxt);
                line = line.substr(nxt+1, -1);

                if (idx == 0) {
                    names.push_back(std::move(data));
                } else if (1 <= idx && idx < 4) {
                    pos.push_back(std::stod(data));
                } else if (4 <= idx && idx < 7) {
                    vel.push_back(std::stod(data));
                }

                ++idx;
            }

            mass.push_back(std::stod(line));
        }

        state::state_type state;
        state.insert(state.end(), pos.begin(), pos.end());
        state.insert(state.end(), vel.begin(), vel.end());

        return { names, state, mass, G };
    }
} }

#endif
