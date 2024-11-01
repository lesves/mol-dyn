#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>
#include <cstdlib>

#include "state.hpp"
#include "integration.hpp"


void dump(std::ofstream& out, const state::State& state) {
	for (std::size_t i = 0; i < state.size(); ++i) {
		out << state.x[i][0] << " " << state.x[i][1] << " ";
	}
	out << "\n";
}


void show(const std::vector<state::State>& states) {
	std::ofstream out("out.txt");

	for (auto it = states.cbegin(); it != states.cend(); ++it) {
		dump(out, *it);
	}

	out << std::flush;

	system("python3 vis.py out.txt");
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

		auto res = integration::parareal(
			rank, n_ranks, // MPI
			0,             // start time
			10,            // end time
			initial,       // initial state
			1000,          // number of segments (must be divisible by the number of ranks)
			10,            // maximum of parareal iters, 
			1e-4           // eps for convergence
		);
		

		if (rank == 0) {
			show(res);
		}

		return MPI_Finalize();
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cout << "Error: Unknown exception." << std::endl;
		return 1;
	}
}
