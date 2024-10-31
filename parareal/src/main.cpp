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


int main(int argc, char** argv) {
	try {
		/*MPI_Status status;
		MPI_Init(&argc, &argv);

		int rank, n_ranks;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

		std::cout << "r" << rank << "/" << n_ranks << "\n";

		return MPI_Finalize();*/

		state::State initial;

		initial.add_particle(
			types::vec({0.97000436, -0.24308753, 0}),
			types::vec({0.466203685, 0.43236573, 0}),
			1
		);
		initial.add_particle(
			types::vec({0, 0, 0}),
			types::vec({-0.93240737,-0.86473146}),
			1
		);
		initial.add_particle(
			types::vec({-0.97000436, 0.24308753}),
			types::vec({0.466203685, 0.43236573}),
			1
		);

		auto states = integration::parareal(0, 10, initial, 1000, 10, 1e-4);

		std::ofstream out("out.txt");

		for (auto it = states.cbegin(); it != states.cend(); ++it) {
			dump(out, *it);
		}

		out << std::flush;

		system("python3 vis.py out.txt");

	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cout << "Error: Unknown exception." << std::endl;
		return 1;
	}
}
