#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Math/Vector.h"

namespace Blaze
{
	struct ColorRGBf;
	struct ColorRGBA;
	struct ColorRGBAf;

	struct BLAZE_API ColorRGB
	{
		uint8 r, g, b;

		constexpr ColorRGB();
		constexpr ColorRGB(uint r, uint g, uint b);
		constexpr ColorRGB(const ColorRGBf&);
		constexpr ColorRGB(const ColorRGBA&);
		constexpr ColorRGB(const ColorRGBAf&);

		constexpr operator Vec3<uint8>() const;

		constexpr bool operator==(const ColorRGB& other) const { return other.r == r && other.g == g && other.b == b; }
		constexpr bool operator!=(const ColorRGB& other) const { return other.r != r || other.g != g || other.b != b; }
	};
	struct BLAZE_API ColorRGBA
	{		
		uint8 r, g, b, a;

		constexpr ColorRGBA();
		constexpr ColorRGBA(uint r, uint g, uint b);
		constexpr ColorRGBA(uint r, uint g, uint b, uint a);
		constexpr ColorRGBA(const ColorRGB& c);
		constexpr ColorRGBA(const ColorRGB& c, uint a);
		constexpr ColorRGBA(const ColorRGBf& c);
		constexpr ColorRGBA(const ColorRGBf& c, uint a);
		constexpr ColorRGBA(const ColorRGBAf& c);
		
		constexpr operator Vec4<uint8>() const;

		constexpr bool operator==(const ColorRGBA& other) const { return other.r == r && other.g == g && other.b == b && other.a == a; }
		constexpr bool operator!=(const ColorRGBA& other) const { return other.r != r || other.g != g || other.b != b || other.a != a; }
	};	
	struct BLAZE_API ColorRGBf
	{
		float r, g, b;

		constexpr ColorRGBf();
		constexpr ColorRGBf(float r, float g, float b);
		constexpr ColorRGBf(const ColorRGB&);
		constexpr ColorRGBf(const ColorRGBA&);
		constexpr ColorRGBf(const ColorRGBAf&);
		
		constexpr operator Vec3f() const;

		constexpr bool operator==(const ColorRGBf& other) const { return other.r == r && other.g == g && other.b == b; }
		constexpr bool operator!=(const ColorRGBf& other) const { return other.r != r || other.g != g || other.b != b; }
	};
	struct BLAZE_API ColorRGBAf
	{
		float r, g, b, a;

		constexpr ColorRGBAf();
		constexpr ColorRGBAf(float r, float g, float b);
		constexpr ColorRGBAf(float r, float g, float b, float a);
		constexpr ColorRGBAf(const ColorRGBf& c);
		constexpr ColorRGBAf(const ColorRGBf& c, float a);
		constexpr ColorRGBAf(const ColorRGB& c);
		constexpr ColorRGBAf(const ColorRGB& c, float a);		
		constexpr ColorRGBAf(const ColorRGBA& c);
		
		constexpr operator Vec4f() const;

		constexpr bool operator==(const ColorRGBAf& other) const { return other.r == r && other.g == g && other.b == b && other.a == a; }
		constexpr bool operator!=(const ColorRGBAf& other) const { return other.r != r || other.g != g || other.b != b || other.a != a; }
	};

	constexpr ColorRGB operator ""_rgb(unsigned long long hex);
	constexpr ColorRGBA operator ""_rgba(unsigned long long hex);
}


#include "BlazeEngine/Core/Common/ColorImpl.h"