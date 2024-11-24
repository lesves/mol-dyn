#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>
#include <cstdlib>

#include "state.hpp"
#include "integration.hpp"
#define VIS


void dump(std::ofstream& out, const state::State& state) {
	auto E_k = state.kinetic_energy();
	auto E_p = state.potential_energy();

	for (std::size_t i = 0; i < state.size(); ++i) {
		out << state.x[i][0] << " " << state.x[i][1] << " " << state.x[i][2] << " ";
	}
	out << E_k << " " << E_p << "\n";
}


void show(const std::vector<state::State>& states) {
	std::ofstream out("out.txt");

	for (auto it = states.cbegin(); it != states.cend(); ++it) {
		dump(out, *it);
	}

	out << std::flush;

	#ifdef VIS
	system("python3 vis.py out.txt");
	#endif
}


int main(int argc, char** argv) {
	try {
		MPI_Status status;
		MPI_Init(&argc, &argv);

		int rank, n_ranks;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

		std::cout << "Rank " << rank << "/" << n_ranks << " running\n";

		state::State initial;

		initial.add_particle(
			types::vec3({0.97000436, -0.24308753, 0}),
			types::vec3({0.466203685, 0.43236573, 0}),
			1
		);
		initial.add_particle(
			types::vec3({0, 0, 0}),
			types::vec3({-0.93240737,-0.86473146}),
			1
		);
		initial.add_particle(
			types::vec3({-0.97000436, 0.24308753}),
			types::vec3({0.466203685, 0.43236573}),
			1
		);

		#ifdef COMPARISON_SERIAL

		if (rank == 0) {
			std::ofstream out("out.txt");
			state::State state = initial;
			for (uint64_t i = 0; i < 1000*1000; ++i) {
				if (i % 1000 == 0) {
					dump(out, state);
				}
				integration::rk4_step(10./(1000.*1000.), state);
			}
			out << std::flush;
			#ifdef VIS
			system("python3 vis.py out.txt");
			#endif
		}
		#else

		auto res = integration::parareal(
			rank, n_ranks, // MPI
			0,             // start time
			10,            // end time
			initial,       // initial state
			1000,          // number of segments (must be divisible by the number of ranks)
			100,           // maximum of parareal iters, 
			1e-4           // eps for convergence
		);

		if (rank == 0) {
			show(res);
		}

		#endif

		return MPI_Finalize();
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cout << "Error: Unknown exception." << std::endl;
		return 1;
	}
}
