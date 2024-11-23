#ifndef PARAREAL_VEC_H
#define PARAREAL_VEC_H

#include <algorithm>
#include <limits>
#include <array>
#include <cmath>


namespace parareal {
	using Dimension = std::size_t;

	template<typename T, Dimension D>
	class Vector {
	private:
		std::array<T, D> components_;

	public:
		static constexpr Dimension Dim = D;

		Vector() : components_({}) {};
		Vector(const std::array<T, D>& components) : components_(components) {};
		Vector(std::array<T, D>&& components) : components_(std::move(components)) {};

		const T& operator[](std::size_t idx) const {
			return components_[idx];
		}

		T& operator[](std::size_t idx) {
			return components_[idx];
		}

		typename std::array<T, D>::iterator begin() {
			return components_.begin();
		}

		typename std::array<T, D>::iterator end() {
			return components_.end();
		}

		typename std::array<T, D>::const_iterator cbegin() const {
			return components_.cbegin();
		}

		typename std::array<T, D>::const_iterator cend() const {
			return components_.cend();
		}

		Vector<T, D> operator+(const Vector<T, D>& other) const {
			Vector<T, D> res;
			for (std::size_t d = 0; d < D; ++d) {
				res[d] = components_[d] + other[d];
			}
			return res;
		}

		void operator+=(const Vector<T, D>& other) {
			for (std::size_t d = 0; d < D; ++d) {
				components_[d] += other[d];
			}
		}

		Vector<T, D> operator-(const Vector<T, D>& other) const {
			Vector<T, D> res;
			for (std::size_t d = 0; d < D; ++d) {
				res[d] = components_[d] - other[d];
			}
			return res;
		}

		void operator-=(const Vector<T, D>& other) {
			for (std::size_t d = 0; d < D; ++d) {
				components_[d] -= other[d];
			}
		}

		Vector<T, D> operator*(const Vector<T, D>& other) const {
			Vector<T, D> res;
			for (std::size_t d = 0; d < D; ++d) {
				res[d] = components_[d] * other[d];
			}
			return res;
		}

		void operator*=(const Vector<T, D>& other) {
			for (std::size_t d = 0; d < D; ++d) {
				components_[d] *= other[d];
			}
		}

		Vector<T, D> operator/(const Vector<T, D>& other) const {
			Vector<T, D> res;
			for (std::size_t d = 0; d < D; ++d) {
				res[d] = components_[d] / other[d];
			}
			return res;
		}

		void operator/=(const Vector<T, D>& other) {
			for (std::size_t d = 0; d < D; ++d) {
				components_[d] /= other[d];
			}
		}

		Vector<T, D> sqrt() const {
			Vector<T, D> res;
			for (std::size_t d = 0; d < D; ++d) {
				res = std::sqrt(components_[d]);
			}
		}

		T norm_squared() const {
			T res = 0;
			for (std::size_t d = 0; d < D; ++d) {
				res += components_[d]*components_[d];
			}
			return res;
		}

		T norm() const {
			return std::sqrt(norm_squared());
		}

		T max() const {
			T res = std::numeric_limits<T>::lowest();
			for (std::size_t d = 0; d < D; ++d) {
				if (res < components_[d]) {
					res = components_[d];
				}
			}
			return res;
		}

		bool has_nan() const {
			for (std::size_t d = 0; d < D; ++d) {
				if (std::isnan(components_[d])) {
					return true;
				}
			}
			return false;
		}

		void clear() {
			for (std::size_t d = 0; d < D; ++d) {
				components_[d] = 0;
			}
		}
	};

	template<typename T, Dimension D>
	inline Vector<T, D> operator*(const Vector<T, D>& one, const T& two) {
		Vector<T, D> res;
		for (std::size_t d = 0; d < D; ++d) {
			res[d] = one[d] * two;
		}
		return res;
	}

	template<typename T, Dimension D>
	inline Vector<T, D> operator*(const T& two, const Vector<T, D>& one) {
		Vector<T, D> res;
		for (std::size_t d = 0; d < D; ++d) {
			res[d] = one[d] * two;
		}
		return res;
	}

	template<typename T, Dimension D>
	inline void operator*=(Vector<T, D>& one, T& two) {
		for (std::size_t d = 0; d < D; ++d) {
			one[d] *= two;
		}
	}

	template<typename T, Dimension D>
	inline Vector<T, D> operator/(const Vector<T, D>& one, const T& two) {
		Vector<T, D> res;
		for (std::size_t d = 0; d < D; ++d) {
			res[d] = one[d] / two;
		}
		return res;
	}

	template<typename T, Dimension D>
	inline Vector<T, D> operator/(const T& two, const Vector<T, D>& one) {
		Vector<T, D> res;
		for (std::size_t d = 0; d < D; ++d) {
			res[d] = one[d] / two;
		}
		return res;
	}

	template<typename T, Dimension D>
	inline void operator/=(Vector<T, D>& one, T& two) {
		for (std::size_t d = 0; d < D; ++d) {
			one[d] /= two;
		}
	}
}

#endif
