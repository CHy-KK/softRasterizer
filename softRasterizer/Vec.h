#pragma once
#ifndef _VEC_H_
#define _VEC_H_
#include <iostream>
#include "mathSR.h"

template<typename T>
class Vec2
{
public:
	union
	{
		struct { T x, y; };
		struct { T u, v; };
		T raw[2];
	};
	Vec2() : raw{ 0, 0 } {}
	Vec2(T e0, T e1) : raw{ e0, e1 } {}
	Vec2(const T v[2]) : raw{ v.x, v.y } {}
	//Vec2f(const Vec2i& v) : x(v.x), y(v.y) {}

	Vec2<T> operator-() const { return Vec2<T>(-x, -y); }
	T operator[](int i) const { return raw[i]; }
	T& operator[](int i) { return raw[i]; }	//	用于左值

	inline Vec2<T> operator+(const Vec2<T>& v) const { return Vec2<T>(x + v.x, y + v.y); }
	inline Vec2<T> operator-(const Vec2<T>& v) const { return Vec2<T>(x - v.x, y - v.y); }
	inline Vec2<T> operator*(const Vec2<T>& v) const { return Vec2<T>(x * v.x, y * v.y); }
	inline Vec2<T> operator*(const float t) const { return Vec2<T>(t * x, t * y); }
	inline Vec2<T> operator/(const float t) const { return Vec2<T>(x / t, y / t); }

	Vec2<T>& operator+=(const Vec2<T>& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	Vec2<T>& operator*=(const float t) {
		x *= t;
		y *= t;
		return *this;
	}

	Vec2<T>& operator/=(const float t) {
		return *this *= 1 / t;
	}

	float length() const { return sqrt(lengthSquared()); }
	float lengthSquared() const { return x * x + y * y; }
	inline static Vec2<T> random() {
		return Vec2<T>(random_double(), random_double());
	}

	inline static Vec2<T> random(double minVal, double maxVal) {
		return Vec2<T>(random_double(minVal, maxVal), random_double(minVal, maxVal));
	}

};

template<typename T>
class Vec3
{
public:
	union 
	{
		struct { T x, y, z; };
		T raw[3];
	};

	Vec3() : raw{ 0, 0, 0 } {}
	Vec3(T e0, T e1, T e2) : raw{ e0, e1, e2 } {}
	Vec3(const T v[3]) : raw{ v.x, v.y, v.z } {}


	Vec3<T> operator-() const { return Vec3<T>(-x, -y, -z); }
	T operator[](int i) const { return raw[i]; }
	T& operator[](int i) { return raw[i]; }	//	用于左值

	inline Vec3<T> operator+(const Vec3<T>& v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<T> operator-(const Vec3<T>& v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<T> operator*(const Vec3<T>& v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
	inline Vec3<T> operator*(const float t) const { return Vec3<T>(t * x, t * y, t * z); }
	inline Vec3<T> operator/(const float t) const { return Vec3<T>(x / t, y / t, z / t);}

	Vec3<T>& operator+=(const Vec3<T>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	
	Vec3<T>& operator+=(const float t) {
		x += t;
		y += t;
		z += t;
		return *this;
	}

	Vec3<T>& operator*=(const float t) {
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	Vec3<T>& operator/=(const float t) {
		return *this *= 1 / t;
	}

	float length() const { return sqrt(lengthSquared()); }
	float lengthSquared() const { return x * x + y * y + z * z; }
	inline static Vec3<T> random() { 
		return Vec3<T>(random_double(), random_double(), random_double()); }

	inline static Vec3<T> random(double minVal, double maxVal) { 
		return Vec3<T>(random_double(minVal, maxVal), random_double(minVal, maxVal), random_double(minVal, maxVal)); 
	}

};
typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;
const Vec3f Vec3f_one(1.f, 1.f, 1.f);
typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;


template<typename T>
inline std::ostream& operator << (std::ostream& out, const Vec3<T>& v) {
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

template<typename T>
inline std::ostream& operator << (std::ostream& out, const Vec2<T>& v) {
	return out << v.x << ' ' << v.y;
}

template<typename T>
inline Vec3<T> operator*(float t, const Vec3<T>& v) { 
	return v * t;
}

template<typename T>
inline Vec2<T> operator*(float t, const Vec2<T>& v) {
	return v * t;
}

template<typename T>
inline float dot(const Vec3<T>& u, const Vec3<T>& v) {
	return u.x * v.x
		+ u.y * v.y
		+ u.z * v.z;
}

template<typename T>
inline float dot(const Vec2<T>& u, const Vec2<T>& v) {
	return u.x * v.x
		+ u.y * v.y;
}

template<typename T>
inline Vec3<T> cross(const Vec3<T>& u, const Vec3<T>& v) {
	return Vec3<T>(u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x);
}

template<typename T>
inline T cross(const Vec2<T>& u, const Vec2<T>& v) {
	return u.x * v.y - u.y * v.x;
}

template<typename T>
inline float distance(const Vec3<T>& u, const Vec3<T>& v) {
	return (u - v).length();
}

template<typename T>
inline float distance(const Vec2<T>& u, const Vec2<T>& v) {
	return (u - v).length();
}

template<typename T>
T lerp(const T& a, const T& b, float t) {
	return (1 - t) * a + t * b;
}

template<typename T>
Vec3f normalize(const Vec3<T>& v) {
	return Vec3f(v) / v.length();
}
	
//Vec3 random_in_unity_sphere();
//Vec3 random_unity_vector();
//Vec3 random_in_unit_dist();
//Vec3 reflect(const Vec3& in, const Vec3& n);
//Vec3 refract(const Vec3& in, const Vec3& n, const double etai);
//double schilick(double cosine, double refIdx);

#endif // VEC3


