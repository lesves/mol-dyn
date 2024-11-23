#include <iostream>
#include <string>

#include <boost/numeric/odeint.hpp>
namespace odeint = boost::numeric::odeint;

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
namespace mpi = boost::mpi;

#include "state.hpp"
#include "simulation.hpp"
#include "parareal.hpp"
#include "io.hpp"


int main(int argc, char** argv) {
	try {
		mpi::environment env;
		mpi::communicator world;
		std::cout << "Rank " << world.rank() << "/" << world.size() << " starting." << std::endl;

        std::vector<std::string> args(argv, argv+argc);
        if (args.size() < 2) {
            std::cerr << args[0] << ": Not enough arguments." << std::endl;
            return 1;
        }

        auto [names, state, masses, G] = parareal::io::load(args[1]);
        std::ofstream outfile("out.txt");

        const std::size_t N = names.size();

        auto coarse_stepper = odeint::runge_kutta4<parareal::state::state_type>();
        auto fine_stepper = odeint::make_controlled(1.0e-3, 1.0e-6, odeint::runge_kutta_dopri5<parareal::state::state_type>());

        /*for (std::size_t i = 0; i < 366*10; ++i) {
            odeint::integrate_adaptive(fine_stepper, parareal::simulation::simulation(G, masses), state, 10.*i, 10.*i + 10., 1.);

            if (world.rank() == 0) {
                parareal::io::output(state, outfile);
            }
        }*/
        //odeint::integrate_adaptive(fine_stepper, parareal::simulation::simulation(G, masses), state, 0., 366., 10., [](const auto& state, const double t){ std::cout << t << std::endl;});
        //parareal::io::output(state, outfile);

        auto res = parareal::parareal(
            world,
            parareal::simulation::simulation(G, masses),
            0., 366.*100.,
            state,
            100,
            1000,
            1e-4,
            coarse_stepper,
            fine_stepper,
            366,
            1.
        );

        for (auto state : res) {
            parareal::io::output(state, outfile);
        }

		return 0;
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cout << "Error: Unknown exception." << std::endl;
		return 1;
	}
}
