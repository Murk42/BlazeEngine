#pragma once

template<typename T>
static inline T MaxOne(const T& n, const T& curve = 1)
{
	if (n < T(0))
		return T(1) / (-n * curve + T(1)) - T(1);
	else 
		return T(1) - T(1) / (n * curve + T(1));
}

template<typename T>
static inline T MaxInf(const T& n, const T& curve = 1)
{
	if (n < T(0))
		return n / (curve * (n + T(1)));
	else
		return n / (curve * (T(1) - n));
}