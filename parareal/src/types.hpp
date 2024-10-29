#ifndef PARAREAL_TYPES_H
#define PARAREAL_TYPES_H

#include "vec.hpp"

namespace types {
	using real = double;
	using vec2 = vec::Vector<real, 2>;
	using vec3 = vec::Vector<real, 3>;

	using vec = vec3;
}

#endif