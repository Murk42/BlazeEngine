#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class fixed
{
	uint64 value;

	constexpr fixed(uint64 value) : value(value) { }
public:
	constexpr fixed() : value(0) { }
	constexpr fixed(const fixed& f) : value(f.value) { }
	constexpr fixed(fixed&& f) : value(f.value) { }

	inline constexpr fixed operator+(const fixed& f) const { return value + f.value; }

	inline constexpr void operator=(const fixed& f) { value = f.value; }
	inline constexpr void operator=(fixed&& f) { value = f.value; }
};