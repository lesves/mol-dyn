#ifndef PARAREAL_IO_H
#define PARAREAL_IO_H

#include <vector>
#include "types.hpp"
#include "state.hpp"

namespace parareal { namespace io {
    void dump(std::ofstream& out, const state::State& state) {
        auto E_k = state.kinetic_energy();
        auto E_p = state.potential_energy();

        for (std::size_t i = 0; i < state.size(); ++i) {
            out << state.x[i][0] << " " << state.x[i][1] << " " << state.x[i][2] << " ";
        }
        out << E_k << " " << E_p << "\n";
    }

    void show(const std::string& fname, const std::vector<state::State>& states) {
        std::ofstream out(fname);

        for (auto it = states.cbegin(); it != states.cend(); ++it) {
            dump(out, *it);
        }

        out << std::flush;
    }
    
    std::tuple<state::Config, std::vector<types::vec3>, std::vector<types::vec3>> load(const std::string& filename) {
        std::vector<std::string> names;
        std::vector<types::vec3> pos;
        std::vector<types::vec3> vel;
        std::vector<double> mass;

        std::ifstream in(filename);

        std::string line;
        std::getline(in, line); // take units + constants
        const double G = std::stod(line.substr(line.find("G=")+2, -1));
        std::getline(in, line); // discard header

        while (std::getline(in, line)) { // read data
            std::size_t idx = 0;

            pos.emplace_back();
            vel.emplace_back();

            for (auto nxt = line.find(','); nxt != -1; nxt = line.find(',')) {
                std::string data = line.substr(0, nxt);
                line = line.substr(nxt+1, -1);

                if (idx == 0) {
                    names.push_back(std::move(data));
                } else if (1 <= idx && idx < 4) {
                    pos.back()[idx-1] = std::stod(data);
                } else if (4 <= idx && idx < 7) {
                    vel.back()[idx-4] = std::stod(data);
                }

                ++idx;
            }

            mass.push_back(std::stod(line));
        }

        state::Config cfg(G, std::move(mass));
        return { cfg, pos, vel };
    }
} }

#endif