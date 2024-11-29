#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

/*#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
namespace mpi = boost::mpi;*/

#include "state.hpp"
#include "integration.hpp"
#include "io.hpp"
//#define USE_PARAREAL


int main(int argc, char** argv) {
	try {
		/*mpi::environment env;
		mpi::communicator world;

		std::cout << "Rank " << world.rank() << "/" << world.size() << " running\n";*/

		/*state::Config cfg(1, {1, 1, 1});
		std::vector x = {
			types::vec3({0.97000436, -0.24308753, 0}),
			types::vec3({0, 0, 0}), 
			types::vec3({-0.97000436, 0.24308753, 0})
		};
		std::vector v = {
			types::vec3({0.466203685, 0.43236573, 0}),
			types::vec3({-0.93240737,-0.86473146, 0}), 
			types::vec3({0.466203685, 0.43236573, 0})
		};
		state::State initial(cfg, std::move(x), std::move(v));*/

		auto [cfg, pos, vel] = io::load("../planets.cfg");
		state::State initial(&cfg, pos, vel);

		#ifdef USE_PARAREAL
			auto res = integration::parareal(
				0,             // start time
				365*100,       // end time
				initial,       // initial state
				4,             // number of segments (must be divisible by the number of ranks)
				1000,          // maximum of parareal iters, 
				1e-12,         // eps for convergence
				integration::FixedStepSolver<integration::VerletStep>(10),
				integration::FixedStepSolver<integration::RK4Step>(10000)
			);
		#else
			auto solver = integration::FixedStepSolver<integration::RK4Step>(40000);
			auto res = solver.integrate_verbose(0, 365*100, initial);
		#endif

		//if (world.rank() == 0) {
		io::show(res);

		#ifdef VIS
		system("python3 vis.py out.txt");
		#endif
		//}

		return 0;
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cout << "Error: Unknown exception." << std::endl;
		return 1;
	}
}
