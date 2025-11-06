#pragma once

#pragma warning( push )
#pragma warning( disable : 26451)
#pragma warning( push )
#pragma warning( disable : 4244)

#include <gcem/gcem.hpp>
#pragma warning( pop )
#pragma warning( pop )

namespace Blaze
{
	template<typename T, size_t S>
	struct Vector;
}

namespace Blaze::Math
{
	constexpr long double PI = 3.14159265359;
	constexpr long double e = 2.71828182846;

	constexpr float Sin(float radians) { return gcem::sin(radians); }
	constexpr double Sin(double radians) { return gcem::sin(radians); }
	constexpr float Cos(float radians) { return gcem::cos(radians); }
	constexpr double Cos(double radians) { return gcem::cos(radians); }
	constexpr float Tan(float radians) { return gcem::tan(radians); }
	constexpr double Tan(double radians) { return gcem::tan(radians); }

	constexpr float Arcsin(float radians) { return gcem::asin(radians); }
	constexpr double Arcsin(double radians) { return gcem::asin(radians); }
	constexpr float Arccos(float radians) { return gcem::acos(radians); }
	constexpr double Arccos(double radians) { return gcem::acos(radians); }
	constexpr float Arctan(float radians) { return gcem::atan(radians); }
	constexpr double Arctan(double radians) { return gcem::atan(radians); }

	constexpr float Sqrt(float value) { return gcem::sqrt(value); }
	constexpr double Sqrt(double value) { return gcem::sqrt(value); }
	constexpr float InvSqrt(float value) { return gcem::inv_sqrt(value); }
	constexpr double InvSqrt(double value) { return gcem::inv_sqrt(value); }

	constexpr float Pow(float base, float exponent) { return gcem::pow(base, exponent); }
	constexpr double Pow(double base, float exponent) { return gcem::pow(base, exponent); }

	constexpr float Ceil(float value) { return gcem::ceil(value); }
	constexpr double Ceil(double value) { return gcem::ceil(value); }
	constexpr float Floor(float value) { return gcem::floor(value); }
	constexpr double Floor(double value) { return gcem::floor(value); }
	template<typename T, uintMem S>
	constexpr Vector<T, S> Ceil(Vector<T, S> vector)
	{
		for (uintMem i = 0; i < S; ++i)
			vector[i] = Ceil(vector[i]);
		return vector;
	}
	template<typename T, uintMem S>
	constexpr Vector<T, S> Floor(Vector<T, S> vector)
	{
		for (uintMem i = 0; i < S; ++i)
			vector[i] = Floor(vector[i]);
		return vector;
	}
}