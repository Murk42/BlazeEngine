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

	template<DecimalType Decimal>
	constexpr Decimal Ceil(Decimal value) { return gcem::ceil(value); }
	template<DecimalType T, uintMem S>
	constexpr Vector<T, S> Ceil(Vector<T, S> vector)
	{
		for (uintMem i = 0; i < S; ++i)
			vector[i] = Ceil(vector[i]);
		return vector;
	}

	template<DecimalType Decimal>
	constexpr Decimal Floor(Decimal value) { return gcem::floor(value); }
	template<DecimalType T, uintMem S>
	constexpr Vector<T, S> Floor(Vector<T, S> vector)
	{
		for (uintMem i = 0; i < S; ++i)
			vector[i] = Floor(vector[i]);
		return vector;
	}
	
	template<DecimalType Decimal>
	constexpr Decimal Round(Decimal value) { return gcem::round(value); }
	template<DecimalType T, uintMem S>
	constexpr Vector<T, S> Round(Vector<T, S> vector)
	{
		for (uintMem i = 0; i < S; ++i)
			vector[i] = Round(vector[i]);
		return vector;
	}

	template<NumberType Number>
	constexpr Number Min(Number a, Number b) { return gcem::min(a, b); }
	template<NumberType T, uintMem S>
	constexpr Vector<T, S> Min(Vector<T, S> a, Vector<T, S> b)
	{
		for (uintMem i = 0; i < S; ++i)
			a[i] = Min(a[i], b[i]);
		return a;
	}

	template<NumberType Number>
	constexpr Number Max(Number a, Number b) { return gcem::max(a, b); }
	template<NumberType T, uintMem S>
	constexpr Vector<T, S> Max(Vector<T, S> a, Vector<T, S> b)
	{
		for (uintMem i = 0; i < S; ++i)
			a[i] = Max(a[i], b[i]);
		return a;
	}

	template<NumberType Number>
	constexpr Number Clamp(Number value, Number min, Number max) { return Max(Min(value, max), min); }
	template<NumberType T, uintMem S>
	constexpr Vector<T, S> Clamp(Vector<T, S> value, Vector<T, S> min, Vector<T, S> max)
	{
		for (uintMem i = 0; i < S; ++i)
			value[i] = Clamp(value[i], min[i], max[i]);

		return value;
	}
}