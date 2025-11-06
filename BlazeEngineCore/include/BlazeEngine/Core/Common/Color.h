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

		ColorRGB() : r(255), g(255), b(255) { }
		ColorRGB(uint8 r, uint8 g, uint8 b) : r(r), g(g), b(b) { }
		explicit ColorRGB(const ColorRGBf&);
		explicit ColorRGB(const ColorRGBA&);
		explicit ColorRGB(const ColorRGBAf&);		
		ColorRGB(uint32 hex);

		explicit operator Vec3<uint8>() const;				

		inline bool operator==(const ColorRGB& other) const { return other.r == r && other.g == g && other.b == b; }
		inline bool operator!=(const ColorRGB& other) const { return other.r != r || other.g != g || other.b != b; }
	};
	struct BLAZE_API ColorRGBA
	{		
		uint8 r, g, b, a;

		ColorRGBA();
		ColorRGBA(uint8 r, uint8 g, uint8 b);
		ColorRGBA(uint8 r, uint8 g, uint8 b, uint8 a);
		explicit ColorRGBA(const ColorRGB& c);
		ColorRGBA(const ColorRGB& c, uint8 a);
		explicit ColorRGBA(const ColorRGBf& c);
		ColorRGBA(const ColorRGBf& c, uint8 a);
		explicit ColorRGBA(const ColorRGBAf& c);
		ColorRGBA(uint32 hex);
		
		explicit operator Vec4<uint8>() const;

		inline bool operator==(const ColorRGBA& other) const { return other.r == r && other.g == g && other.b == b && other.a == a; }
		inline bool operator!=(const ColorRGBA& other) const { return other.r != r || other.g != g || other.b != b || other.a != a; }
	};	
	struct BLAZE_API ColorRGBf
	{
		float r, g, b;

		ColorRGBf();
		ColorRGBf(float r, float g, float b);
		explicit ColorRGBf(const ColorRGB&);
		explicit ColorRGBf(const ColorRGBA&);
		explicit ColorRGBf(const ColorRGBAf&);				
		ColorRGBf(uint32 hex);
		
		explicit operator Vec3f() const;

		inline bool operator==(const ColorRGBf& other) const { return other.r == r && other.g == g && other.b == b; }
		inline bool operator!=(const ColorRGBf& other) const { return other.r != r || other.g != g || other.b != b; }
	};
	struct BLAZE_API ColorRGBAf
	{
		float r, g, b, a;

		ColorRGBAf();
		ColorRGBAf(float r, float g, float b);
		ColorRGBAf(float r, float g, float b, float a);
		explicit ColorRGBAf(const ColorRGBf& c);
		ColorRGBAf(const ColorRGBf& c, float a);
		explicit ColorRGBAf(const ColorRGB& c);
		ColorRGBAf(const ColorRGB& c, float a);		
		explicit ColorRGBAf(const ColorRGBA& c);
		ColorRGBAf(uint hex);
		
		operator Vec4f() const;

		inline bool operator==(const ColorRGBAf& other) const { return other.r == r && other.g == g && other.b == b && other.a == a; }
		inline bool operator!=(const ColorRGBAf& other) const { return other.r != r || other.g != g || other.b != b || other.a != a; }
	};	
}