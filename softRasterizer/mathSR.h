#pragma once
#ifndef _MATH_SR_H_
#define _MATH_SR_H_

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <cstdlib>
#include <functional>
#include <random>

using std::shared_ptr;
using std::make_shared;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180;
}

inline double random_double() {
	// 把随机数规范到[0,1)
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
	// 范围随机
	return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
	return static_cast<int>(random_double(min, max + 1));
}

inline float clamp(float x, float min, float max) {
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

template<typename T>
inline T ffmin(T a, T b) { return a <= b ? a : b; }
template<typename T>
inline T ffmax(T a, T b) { return a >= b ? a : b; }

#endif // !_MATH_SR_H_

