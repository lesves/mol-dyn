#ifndef PARAREAL_CONFIG_H
#define PARAREAL_CONFIG_H

#include "types.hpp"

namespace config {
	struct Config {
		types::real dist_unit, time_unit, mass_unit;
		types::real G;
		types::real eps;

		Config(types::real dist_unit, types::real time_unit, types::real mass_unit, types::real eps, types::real G0) : 
			dist_unit(dist_unit), 
			time_unit(time_unit), 
			mass_unit(mass_unit),
			eps(eps),
			G(G0 * (time_unit*time_unit) / (dist_unit*dist_unit*dist_unit) * mass_unit)
			{}
	};
}

#endif