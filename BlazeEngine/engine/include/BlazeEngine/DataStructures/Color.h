#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
{
	struct ColorRGBf;
	struct ColorRGBAf;

	struct ColorRGB
	{
		uint8 r, g, b;

		constexpr inline ColorRGB() : r(255), g(255), b(255) { }
		constexpr inline ColorRGB(uint8 r, uint8 g, uint8 b) : r(r), g(g), b(b) { }
		constexpr inline ColorRGB(const ColorRGBf&);

		constexpr inline Vec3<uint8> ToVector() const { return Vec3<uint8>(r, g, b); }

		constexpr inline operator Vec3<uint8>() const { return ToVector(); }
		constexpr inline operator Vec3f() const;

		constexpr inline bool operator==(const ColorRGB& other) const { return other.r == r && other.g == g && other.b == b; }
		constexpr inline bool operator!=(const ColorRGB& other) const { return other.r != r || other.g != g || other.b != b; }
	};
	struct ColorRGBA
	{		
		uint8 r, g, b, a;

		constexpr inline ColorRGBA() : r(255), g(255), b(255), a(255) { }
		constexpr inline ColorRGBA(uint8 r, uint8 g, uint8 b, uint8 a) : r(r), g(g), b(b), a(a) { }
		constexpr inline ColorRGBA(ColorRGB c, uint8 a) : r(c.r), g(c.g), b(c.b), a(a) { }
		constexpr inline ColorRGBA(const ColorRGBAf&);

		constexpr inline Vec4<uint8> ToVector() const { return Vec4<uint8>(r, g, b, a); }

		constexpr inline operator Vec4<uint8>() const { return ToVector(); }
		constexpr inline operator Vec4f() const;

		constexpr inline bool operator==(const ColorRGBA& other) const { return other.r == r && other.g == g && other.b == b && other.a == a; }
		constexpr inline bool operator!=(const ColorRGBA& other) const { return other.r != r || other.g != g || other.b != b || other.a != a; }
	};	
	struct ColorRGBf
	{
		float r, g, b;

		constexpr inline ColorRGBf() : r(1.0f), g(1.0f), b(1.0f) { }
		constexpr inline ColorRGBf(float r, float g, float b) : r(r), g(g), b(b) { }
		constexpr inline ColorRGBf(const ColorRGB& other) : r((float)other.r / 255.0f), g((float)other.g / 255.0f), b((float)other.b / 255.0f) { }

		constexpr inline Vec3f ToVector() const { return Vec3f(r, g, b); }

		constexpr inline operator Vec3<uint8>() const;
		constexpr inline operator Vec3f() const { return ToVector(); }

		constexpr inline bool operator==(const ColorRGBf& other) const { return other.r == r && other.g == g && other.b == b; }
		constexpr inline bool operator!=(const ColorRGBf& other) const { return other.r != r || other.g != g || other.b != b; }
	};
	struct ColorRGBAf
	{
		float r, g, b, a;

		constexpr inline ColorRGBAf() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }
		constexpr inline ColorRGBAf(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
		constexpr inline ColorRGBAf(ColorRGBf c, float a) : r(c.r), g(c.g), b(c.b), a(a) { }
		constexpr inline ColorRGBAf(const ColorRGBA& other) : r(float(other.r) / 255.0f), g(float(other.g) / 255.0f), b(float(other.b) / 255.0f), a(float(other.a) / 255.0f) { }

		constexpr inline Vec4f ToVector() const { return Vec4f(r, g, b, a); }

		constexpr inline operator Vec4<uint8>() const;
		constexpr inline operator Vec4f() const { return ToVector(); }

		constexpr inline bool operator==(const ColorRGBAf& other) const { return other.r == r && other.g == g && other.b == b && other.a == a; }
		constexpr inline bool operator!=(const ColorRGBAf& other) const { return other.r != r || other.g != g || other.b != b || other.a != a; }
	};

	constexpr inline Blaze::ColorRGB::ColorRGB(const ColorRGBf& other) : r(float(other.r) * 255.0f), g(float(other.g) * 255.0f), b(float(other.b) * 255.0f) { }
	constexpr inline Blaze::ColorRGBA::ColorRGBA(const ColorRGBAf& other) : r(float(other.r) * 255.0f), g(float(other.g) * 255.0f), b(float(other.b) * 255.0f), a(float(other.a) * 255.0f) { }

	constexpr inline ColorRGB::operator Vec3f() const { return ColorRGBf(*this); }
	constexpr inline ColorRGBA::operator Vec4f() const { return ColorRGBAf(*this); }
	constexpr inline ColorRGBf::operator Vec3<uint8>() const { return ColorRGB(*this); }
	constexpr inline ColorRGBAf::operator Vec4<uint8>() const { return ColorRGBA(*this); }

	namespace Colors
	{
		constexpr ColorRGBA White = ColorRGBA(255, 255, 255, 255);
		constexpr ColorRGBA Black = ColorRGBA(0, 0, 0, 255);
		constexpr ColorRGBA Red = ColorRGBA(255, 0, 0, 255);
		constexpr ColorRGBA Green = ColorRGBA(0, 255, 0, 255);
		constexpr ColorRGBA Blue = ColorRGBA(0, 0, 255, 255);
	}
}