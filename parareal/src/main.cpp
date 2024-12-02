#include <iostream>
#include <fstream>
#include <vector>

#include <cstdlib>
#include <sstream>

#include <cxxopts.hpp>

#include "state.hpp"
#include "integration.hpp"
#include "io.hpp"


int main(int argc, char** argv) {
	try {
		cxxopts::Options options("parareal", "Simulate the solar system using the parallel-in-time algorithm, parareal");
	    options.add_options()
	        ("i,in", "Input file", cxxopts::value<std::string>())
	        ("o,out", "Output file", cxxopts::value<std::string>())
	        ("n,nseg", "Number of segments", cxxopts::value<std::size_t>())
	        ("t,time", "Simulation length", cxxopts::value<double>())
	        ("coarse-steps", "Number of steps of the coarse integrator", cxxopts::value<std::size_t>())
	        ("fine-steps", "Number of steps of the coarse integrator", cxxopts::value<std::size_t>())
	        ("max-iters", "Maximum number of parareal iterations", cxxopts::value<std::size_t>()->default_value("1000"))
	        ("eps", "Epsilon for parareal convergence", cxxopts::value<double>()->default_value("1e-5"))
	        ("vis", "Boolean", cxxopts::value<bool>()->default_value("false"))
	        ("serial", "Switch to serial algorithm (for comparison)", cxxopts::value<bool>()->default_value("false"))
	        ("log-period", "Log every `log_period` timesteps", cxxopts::value<std::size_t>()->default_value("1"))
	    ;
	    auto args = options.parse(argc, argv);

	    for (auto&& arg : { "in", "out", "nseg", "coarse-steps", "fine-steps" }) {
		    if (!args.count(arg)) {
		    	std::cerr << argv[0] << ": missing required argument: " << arg << std::endl;
		    	return 1;
		    }
	    }

		auto [cfg, pos, vel] = parareal::io::load(args["in"].as<std::string>());
		parareal::state::State initial(&cfg, pos, vel);

		std::vector<parareal::state::State> res;

		auto nseg = args["nseg"].as<std::size_t>();
		auto time = args["time"].as<double>();
		auto csteps = args["coarse-steps"].as<std::size_t>();
		auto fsteps = args["fine-steps"].as<std::size_t>();
		auto log_period = args["log-period"].as<std::size_t>();

		if (args["serial"].as<bool>()) {
			auto steps = nseg*fsteps;

			auto solver = parareal::integration::solver_with_log_period<parareal::integration::RK4Fixed>(steps, log_period);
			res = solver.integrate_verbose(0, time, initial);
		} else {
			res = parareal::integration::parareal(
				0,                                   // start time
				time,                                // end time
				initial,                             // initial state
				nseg,                                // number of segments (must be divisible by the number of ranks)
				args["max-iters"].as<std::size_t>(), // maximum of parareal iters, 
				args["eps"].as<double>(),            // eps for convergence
				parareal::integration::VelocityVerlet(csteps),
				parareal::integration::solver_with_log_period<parareal::integration::RK4Fixed>(fsteps, log_period)
			);
		}

		std::string outfile = args["out"].as<std::string>();
		parareal::io::show(outfile, res);

		if (args["vis"].as<bool>()) {
			std::stringstream ss;
			ss << "python3 vis.py " << outfile;
			system(ss.str().c_str());
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
