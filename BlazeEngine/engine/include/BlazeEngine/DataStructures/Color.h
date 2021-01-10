#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
{
	struct Colorb;

	struct BLAZE_API Color
	{
		float r, g, b, a;
		constexpr Color() : r(0), g(0), b(0), a(1) { }
		constexpr Color(const float& c) : r (c), g(c), b(c), a(1) { }
		constexpr Color(const int& c) : r(float(c) / 255), g(float(c) / 255), b(float(c) / 255), a(1) { }
		constexpr Color(const float& r, const float& g, const float& b, const float& a = 1.0f) : r(r), g(g), b(b), a(a) { }
		constexpr Color(const int& r, const int& g, const int& b, const int& a = 255) : r(float(r) / 256.0f), g(float(g) / 256.0f), b(float(b) / 256.0f), a(float(a) / 256.0f) { }

		constexpr inline bool operator==(const Color& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
		constexpr inline bool operator!=(const Color& c) const { return r != c.r || g != c.g || b != c.b || a != c.a; }
		constexpr inline void operator=(const Colorb& c);

		Vec4f& ToVector() { return *(Vec4f*)this; }
		Vec4f& ToVector() const { return *(Vec4f*)this; }
	};

	struct BLAZE_API Colorb
	{
		ubyte a, b, g, r;
		constexpr Colorb() : r(0), g(0), b(0), a(1) { }
		constexpr Colorb(const int& c) : r(c), g(c), b(c), a(c) { }
		constexpr Colorb(const int& r, const int& g, const int& b, const int& a = 255) : r(r), g(g), b(b), a(a) { }
		constexpr Colorb(const float& r, const float& g, const float& b, const float& a = 1.0f) : r(byte(r * 255.0f)), g(byte(g * 255.0f)), b(byte(b * 255.0f)), a(byte(a * 255.0f)) { }

		constexpr inline bool operator==(const Colorb& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
		constexpr inline bool operator!=(const Colorb& c) const { return r != c.r || g != c.g || b != c.b || a != c.a; }
		constexpr inline void operator=(const Color& c) { r = byte(c.r * 255); g = byte(c.g * 255); b = byte(c.b * 255); a = byte(c.a * 255); }

		uint32& ToUnsigned() { return *(uint32*)this; }
		const uint32& ToUnsigned() const { return *(uint32*)this; }
	};

	constexpr inline void Color::operator=(const Colorb& c) 
	{ 
		r = float(c.r) / 256.0f; 
		g = float(c.g) / 256.0f; 
		b = float(c.b) / 256.0f; 
		a = float(c.a) / 256.0f; 
	}
}